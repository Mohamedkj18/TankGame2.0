import sys
from game_loader import GameData
from renderer import GameRenderer

def main():
    if len(sys.argv) < 2:
        print("Usage: python main.py <output_file>")
        sys.exit(1)
    output_file = sys.argv[1]
    visualization_file = "data/visualization.txt"
    data = GameData(visualization_file, output_file)
    renderer = GameRenderer(data, output_file)
    renderer.run()

if __name__ == "__main__":
    main()