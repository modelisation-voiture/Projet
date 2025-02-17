#include <SDL2/SDL.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>
#include <iostream>

using json = nlohmann::json;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Circuit {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<std::pair<SDL_Point, SDL_Point>> borders;
    SDL_Point startPoint;

public:
    Circuit(const std::string& jsonFile) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Erreur SDL : " << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }

        window = SDL_CreateWindow("Circuit SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Erreur fenêtre SDL : " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(EXIT_FAILURE);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Erreur renderer SDL : " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(EXIT_FAILURE);
        }

        loadFromJson(jsonFile);
    }

    ~Circuit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void loadFromJson(const std::string& jsonFile) {
        std::ifstream file(jsonFile);
        if (!file) {
            std::cerr << "Erreur : impossible d'ouvrir " << jsonFile << std::endl;
            exit(EXIT_FAILURE);
        }

        json data;
        try {
            file >> data;
        } catch (json::parse_error& e) {
            std::cerr << "Erreur JSON : " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

        startPoint.x = data["circuit"]["points_depart"]["x"].get<int>();
        startPoint.y = data["circuit"]["points_depart"]["y"].get<int>();

        for (const auto& border : data["circuit"]["bordures"]) {
            SDL_Point p1 = { border["x1"].get<int>(), border["y1"].get<int>() };
            SDL_Point p2 = { border["x2"].get<int>(), border["y2"].get<int>() };
            borders.emplace_back(p1, p2);
        }
    }

    void run() {
        bool running = true;
        SDL_Event event;

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                    running = false;
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Dessiner les bordures en blanc
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (const auto& border : borders) {
                SDL_RenderDrawLine(renderer, border.first.x, border.first.y, border.second.x, border.second.y);
            }

            // Dessiner le point de départ en vert
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect startRect = { startPoint.x - 5, startPoint.y - 5, 10, 10 };
            SDL_RenderFillRect(renderer, &startRect);

            SDL_RenderPresent(renderer);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_json>\n";
        return EXIT_FAILURE;
    }

    Circuit circuit(argv[1]);
    circuit.run();
    return 0;
}
