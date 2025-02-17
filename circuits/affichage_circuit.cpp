#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Circuit {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* asphaltTexture;
    SDL_Texture* borderTexture;
    SDL_Texture* grassTexture;
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

        // Charger les textures
        asphaltTexture = loadTexture("textures/asphalt.png");
        borderTexture = loadTexture("textures/borders.png");
        grassTexture = loadTexture("textures/grass.png");

        loadFromJson(jsonFile);
    }

    ~Circuit() {
        SDL_DestroyTexture(asphaltTexture);
        SDL_DestroyTexture(borderTexture);
        SDL_DestroyTexture(grassTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Texture* loadTexture(const std::string& path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Erreur : Impossible de charger l'image " << path << " : " << IMG_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
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

    void drawCurve(SDL_Point p0, SDL_Point p1, SDL_Point p2, SDL_Color color) {
        for (double t = 0; t <= 1; t += 0.01) {
            int x = (1 - t) * (1 - t) * p0.x + 2 * (1 - t) * t * p1.x + t * t * p2.x;
            int y = (1 - t) * (1 - t) * p0.y + 2 * (1 - t) * t * p1.y + t * t * p2.y;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    void render() {
        // Afficher le fond (herbe)
        SDL_Rect bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, grassTexture, NULL, &bg);

        // Dessiner les bordures courbées
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (size_t i = 0; i < borders.size() - 1; i++) {
            drawCurve(borders[i].first, borders[i].second, borders[i + 1].first, {255, 255, 255});
        }

        // Dessiner l'asphalte
        for (const auto& border : borders) {
            SDL_Rect rect = {border.first.x, border.first.y, border.second.x - border.first.x, border.second.y - border.first.y};
            SDL_RenderCopy(renderer, asphaltTexture, NULL, &rect);
        }

        // Dessiner le point de départ
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect startRect = {startPoint.x - 5, startPoint.y - 5, 10, 10};
        SDL_RenderFillRect(renderer, &startRect);
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

            SDL_RenderClear(renderer);
            render();
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
