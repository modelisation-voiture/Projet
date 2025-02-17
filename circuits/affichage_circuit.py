import json

def load_circuit(json_file):
    with open(json_file, 'r') as f:
        return json.load(f)

def display_circuit(circuit_data):
    width = circuit_data["circuit"]["dimensions"]["largeur"] // 10
    height = circuit_data["circuit"]["dimensions"]["hauteur"] // 10
    grid = [[' ' for _ in range(width)] for _ in range(height)]
    
    # Bordures
    for border in circuit_data["circuit"]["bordures"]:
        x1, y1 = min(border["x1"] // 10, width - 1), min(border["y1"] // 10, height - 1)
        x2, y2 = min(border["x2"] // 10, width - 1), min(border["y2"] // 10, height - 1)
        
        if y1 == y2:
            for x in range(min(x1, x2), max(x1, x2) + 1):
                if 0 <= x < width and 0 <= y1 < height:
                    grid[y1][x] = '-'
        if x1 == x2:
            for y in range(min(y1, y2), max(y1, y2) + 1):
                if 0 <= x1 < width and 0 <= y < height:
                    grid[y][x1] = '|'
    
    # Point de départ
    start_x = min(circuit_data["circuit"]["points_depart"]["x"] // 10, width - 1)
    start_y = min(circuit_data["circuit"]["points_depart"]["y"] // 10, height - 1)
    if 0 <= start_x < width and 0 <= start_y < height:
        grid[start_y][start_x] = 'S'
    
    # Checkpoints
    for checkpoint in circuit_data["circuit"]["checkpoints"]:
        cx = min(checkpoint["x"] // 10, width - 1)
        cy = min(checkpoint["y"] // 10, height - 1)
        if 0 <= cx < width and 0 <= cy < height:
            grid[cy][cx] = 'C'
    
    # Affichage du circuit
    for row in grid:
        print(''.join(row))

if __name__ == "__main__":
    circuit = load_circuit("circuuit1.json")
    display_circuit(circuit)