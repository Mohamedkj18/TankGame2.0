class GameData:
    def __init__(self, visualization_path, output_path):
        self.visual_steps = self._load_visualization(visualization_path)
        self.tank_actions = self._load_output(output_path)

    def _load_visualization(self, path):
        steps = []
        with open(path, "r") as f:
            step = []
            for line in f:
                line = line.strip()
                if line.startswith("==="):
                    if step:
                        steps.append(step)
                        step = []
                elif line:
                    step.append(line)
            if step:
                steps.append(step)
        return steps

    def _load_output(self, path):
        actions = []
        with open(path, "r") as f:
            for line in f:
                if line.startswith("Player") or line.startswith("Tie") or line.startswith("Game Over"):
                    break
                actions.append([s.strip() for s in line.strip().split(",")])
        return actions