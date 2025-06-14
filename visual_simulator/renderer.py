import pygame
import math
import os

TILE_SIZE = 32
FONT_SIZE = 18

COLORS = {
    '#': (60, 60, 60),
    '/': (120, 120, 120),
    '*': (0, 255, 255),
    '.': (255, 255, 255),
    ' ': (255, 255, 255)
}

ROTATION_ANGLE = {
    0: 270,
    45: 315,
    90: 0,
    135: 45,
    180: 90,
    225: 135,
    270: 180,
    315: 225
}

class GameRenderer:
    def __init__(self, data, output_file_path):
        import pygame
        self.data = data
        self.output_file_path = output_file_path
        self.step = 0
        self.width = len(data.visual_steps[0][0])
        self.height = len(data.visual_steps[0])
        self.jump_input = ""
        self.input_active = False
        self.replay_mode = False
        self.replay_paused = False
        self.replay_exit = False
        pygame.init()

        # --- Dynamically adjust TILE_SIZE to fit screen ---
        info = pygame.display.Info()
        screen_width, screen_height = info.current_w, info.current_h
        max_tile_w = screen_width // self.width
        max_tile_h = (screen_height - 100) // self.height
        global TILE_SIZE
        TILE_SIZE = min(TILE_SIZE, max_tile_w, max_tile_h)
        # --- End dynamic TILE_SIZE ---

        self.font = pygame.font.SysFont(None, FONT_SIZE)
        self.input_box = pygame.Rect(200, self.height * TILE_SIZE + 10, 100, 30)
        # Ensure window is tall enough for buttons and input
        self.window = pygame.display.set_mode((self.width * TILE_SIZE, self.height * TILE_SIZE + 100))
        pygame.display.set_caption("Tank Game Visualizer")
        # Define buttons (y position below input box)
        self.button_replay = pygame.Rect(10, self.height * TILE_SIZE + 60, 80, 30)
        self.button_stop = pygame.Rect(100, self.height * TILE_SIZE + 60, 80, 30)
        self.button_restart = pygame.Rect(190, self.height * TILE_SIZE + 60, 80, 30)
        self.button_exit = pygame.Rect(280, self.height * TILE_SIZE + 60, 80, 30)
    
        self.tank_red = pygame.transform.scale(pygame.image.load("visual_simulator/assets/tank_red.png").convert_alpha(), (TILE_SIZE, TILE_SIZE))
        self.tank_blue = pygame.transform.scale(pygame.image.load("visual_simulator/assets/tank_blue.png").convert_alpha(), (TILE_SIZE, TILE_SIZE))
        self.mine_icon = pygame.transform.scale(pygame.image.load("visual_simulator/assets/mine_skull.png").convert_alpha(), (TILE_SIZE, TILE_SIZE))
    
        # Assign a unique ID to each tank at initialization
        self.tank_initial_order = []
        self.tank_id_map = {}  # (y, x, ch) -> tank_id
        tank_id_counter = 0
        for y, row in enumerate(self.data.visual_steps[0]):
            for x, ch in enumerate(row):
                if ch in ('1', '2'):
                    self.tank_initial_order.append((y, x, ch))
                    self.tank_id_map[(y, x, ch)] = tank_id_counter
                    tank_id_counter += 1
        self.tank_directions = self._compute_tank_directions(self.output_file_path)
        self.tank_id_history = self._compute_tank_id_history()
    
    def _compute_tank_directions(self, filepath):
        angle_map = {
            'RotateLeft45': -45,
            'RotateLeft90': -90,
            'RotateRight45': 45,
            'RotateRight90': 90
        }

        with open(filepath, 'r') as f:
            lines = [line.strip() for line in f if line and not line.startswith("Player") and not line.startswith("Tie")]

        # Assign initial directions and IDs
        tank_ids = []
        initial_directions = []
        for y, row in enumerate(self.data.visual_steps[0]):
            for x, ch in enumerate(row):
                if ch in ('1', '2'):
                    tank_ids.append(self.tank_id_map[(y, x, ch)])
                    if ch == '1':
                        initial_directions.append(270)  # Player 1 tanks start facing left (270°)
                    else:
                        initial_directions.append(90)   # Player 2 tanks start facing right (90°)

        tank_angles = {tid: angle for tid, angle in zip(tank_ids, initial_directions)}
        all_steps = [tank_angles.copy()]

        for step_idx, line in enumerate(lines):
            moves = [m.strip() for m in line.split(",")]
            prev_directions = all_steps[-1]
            curr_directions = prev_directions.copy()
            for i, move in enumerate(moves):
                tid = tank_ids[i]
                direction = prev_directions[tid]
                for cmd in move.split():
                    if cmd in angle_map:
                        direction = (direction + angle_map[cmd]) % 360
                curr_directions[tid] = direction
            all_steps.append(curr_directions.copy())

        return all_steps

    def _compute_tank_id_history(self):
        # For each step, map (y, x, ch) to tank_id with one-to-one matching
        tank_id_history = []
        prev_map = self.tank_id_map.copy()
        next_id = max(prev_map.values(), default=-1) + 1

        for step_idx, board in enumerate(self.data.visual_steps):
            step_map = {}
            used_prev = set()
            used_curr = set()
            # Gather current tanks
            curr_tanks = [(y, x, ch) for y, row in enumerate(board) for x, ch in enumerate(row) if ch in ('1', '2')]
            prev_tanks = list(prev_map.keys())

            # Build cost matrix for matching: only match same type
            cost_matrix = []
            for curr in curr_tanks:
                row = []
                for prev in prev_tanks:
                    if curr[2] == prev[2]:
                        row.append(abs(curr[0] - prev[0]) + abs(curr[1] - prev[1]))
                    else:
                        row.append(1e6)  # Large cost for different type
                cost_matrix.append(row)

            # Greedy matching: for each curr, find closest unmatched prev
            matched_prev = set()
            matched_curr = set()
            assignments = {}
            for _ in range(min(len(curr_tanks), len(prev_tanks))):
                min_cost = 1e6
                min_i = min_j = -1
                for i, curr in enumerate(curr_tanks):
                    if i in matched_curr:
                        continue
                    for j, prev in enumerate(prev_tanks):
                        if j in matched_prev:
                            continue
                        if cost_matrix[i][j] < min_cost:
                            min_cost = cost_matrix[i][j]
                            min_i, min_j = i, j
                if min_i != -1 and min_j != -1 and min_cost < 1e6:
                    curr = curr_tanks[min_i]
                    prev = prev_tanks[min_j]
                    tid = prev_map[prev]
                    step_map[curr] = tid
                    matched_curr.add(min_i)
                    matched_prev.add(min_j)
            # Assign new IDs to unmatched tanks
            for i, curr in enumerate(curr_tanks):
                if i not in matched_curr:
                    step_map[curr] = next_id
                    next_id += 1
            tank_id_history.append(step_map)
            prev_map = step_map.copy()
        return tank_id_history

    def run(self):
        clock = pygame.time.Clock()
        running = True
        replay_timer = 0
        REPLAY_INTERVAL = 200  # milliseconds between steps during replay

        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    if self.input_active:
                        if event.key == pygame.K_RETURN:
                            if self.jump_input.isdigit():
                                jump_to = int(self.jump_input)
                                if 0 <= jump_to < len(self.data.visual_steps):
                                    self.step = jump_to
                            self.jump_input = ""
                            self.input_active = False
                        elif event.key == pygame.K_BACKSPACE:
                            self.jump_input = self.jump_input[:-1]
                        else:
                            if event.unicode.isdigit():
                                self.jump_input += event.unicode
                    else:
                        if event.key == pygame.K_RIGHT or event.key == pygame.K_PERIOD:
                            self.step = min(self.step + 1, len(self.data.visual_steps) - 1)
                            self.replay_mode = False
                        elif event.key == pygame.K_LEFT or event.key == pygame.K_COMMA:
                            self.step = max(self.step - 1, 0)
                            self.replay_mode = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if self.input_box.collidepoint(event.pos):
                        self.input_active = True
                    else:
                        self.input_active = False
                        # Button click handling
                        if self.button_replay.collidepoint(event.pos):
                            self.replay_mode = True
                            self.replay_paused = False
                        elif self.button_stop.collidepoint(event.pos):
                            self.replay_paused = True
                        elif self.button_restart.collidepoint(event.pos):
                            self.step = 0
                            self.replay_mode = True
                            self.replay_paused = False
                        elif self.button_exit.collidepoint(event.pos):
                            running = False

            # Handle replay logic
            if self.replay_mode and not self.replay_paused:
                replay_timer += clock.get_time()
                if replay_timer >= REPLAY_INTERVAL:
                    if self.step < len(self.data.visual_steps) - 1:
                        self.step += 1
                        replay_timer = 0
                    else:
                        self.replay_mode = False
                        self.replay_paused = False
                        replay_timer = 0
            else:
                replay_timer = 0

            self.render_step(self.step)
            self.render_ui_background()
            self.render_buttons()
            self.render_navigation_tip()
            pygame.display.flip()
            clock.tick(60)
        pygame.quit()

    def render_step(self, step_index):
        self.window.fill((0, 0, 0))
        board = self.data.visual_steps[step_index]
        dir_index = step_index
        directions = self.tank_directions[min(dir_index, len(self.tank_directions) - 1)]
        # For each tank on the board, find its index in the initial order and use that to get the correct direction
        current_tanks = [(y, x, char) for y, line in enumerate(board) for x, char in enumerate(line) if char in ('1', '2')]
        tank_id_map = self.tank_id_history[step_index]
        tank_directions = self.tank_directions[step_index]
        for y, x, char in current_tanks:
            tid = tank_id_map.get((y, x, char), None)
            if tid is not None and tid in tank_directions:
                angle = tank_directions[tid] % 360
                tank_img = self.tank_red if char == '1' else self.tank_blue
                rotated = pygame.transform.rotate(tank_img, -angle)
                self.window.blit(rotated, (x * TILE_SIZE, y * TILE_SIZE))
        # Draw mines and other tiles
        for y, line in enumerate(board):
            for x, char in enumerate(line):
                if char == '@':
                    self.window.blit(self.mine_icon, (x * TILE_SIZE, y * TILE_SIZE))
                elif char not in ('1', '2'):
                    color = COLORS.get(char, (255, 255, 255))
                    pygame.draw.rect(self.window, color, pygame.Rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE))

        status = f"Step {self.step} | ←/→ to navigate"
        text = self.font.render(status, True, (255, 255, 255))
        self.window.blit(text, (10, self.height * TILE_SIZE + 10))

        pygame.draw.rect(self.window, (255, 255, 255), self.input_box, 2)
        input_text = self.font.render(self.jump_input, True, (255, 255, 255))
        self.window.blit(input_text, (self.input_box.x + 5, self.input_box.y + 5))

    def render_ui_background(self):
        # Draw a black rectangle for the UI area below the board
        pygame.draw.rect(
            self.window,
            (0, 0, 0),
            pygame.Rect(
                0,
                self.height * TILE_SIZE,
                self.width * TILE_SIZE,
                100
            )
        )

    def render_buttons(self):
        # Draw the replay control buttons
        button_color = (80, 80, 80)
        text_color = (255, 255, 255)
        pygame.draw.rect(self.window, button_color, self.button_replay)
        pygame.draw.rect(self.window, button_color, self.button_stop)
        pygame.draw.rect(self.window, button_color, self.button_restart)
        pygame.draw.rect(self.window, button_color, self.button_exit)

        replay_text = self.font.render("Replay", True, text_color)
        stop_text = self.font.render("Stop", True, text_color)
        restart_text = self.font.render("Restart", True, text_color)
        exit_text = self.font.render("Exit", True, text_color)

        self.window.blit(replay_text, (self.button_replay.x + 10, self.button_replay.y + 5))
        self.window.blit(stop_text, (self.button_stop.x + 18, self.button_stop.y + 5))
        self.window.blit(restart_text, (self.button_restart.x + 5, self.button_restart.y + 5))
        self.window.blit(exit_text, (self.button_exit.x + 18, self.button_exit.y + 5))

    def render_navigation_tip(self):
        # Draw a tip for navigation below the buttons
        tip_text = self.font.render("Tip: Use < or > to navigate steps.", True, (200, 200, 200))
        tip_y = self.height * TILE_SIZE + 95
        self.window.blit(tip_text, (10, tip_y))
