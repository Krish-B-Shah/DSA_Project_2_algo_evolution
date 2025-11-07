#if defined(__has_include)
  #if __has_include(<SFML/Graphics.hpp>) && __has_include(<SFML/Window.hpp>)
    #include <SFML/Graphics.hpp>
    #include <SFML/Window.hpp>
  #else
    // SFML headers not found —provide minimal compile-time stubs so the file
    // can be parsed/compiled in environments without SFML installed.
    #include <cstdint>
    #include <cstddef>
    #include <string>
    #include <vector>

    namespace sf {
        struct Color { uint8_t r,g,b,a; Color(uint8_t rr=0,uint8_t gg=0,uint8_t bb=0,uint8_t aa=255):r(rr),g(gg),b(bb),a(aa){} };
        struct Vector2f { float x,y; Vector2f(float xx=0,float yy=0):x(xx),y(yy){} };
        struct Vector2u { unsigned int x,y; Vector2u(unsigned int xx=0,unsigned int yy=0):x(xx),y(yy){} };

        struct Vertex { Vector2f position; Color color; Vertex() : position(), color() {} Vertex(const Vector2f& p, const Color& c) : position(p), color(c) {} };

        class Shape {
        public:
            virtual ~Shape() {}
            void setPosition(float, float) {}
            void setFillColor(const Color&) {}
            void setOutlineColor(const Color&) {}
            void setOutlineThickness(float) {}
            float getRadius() const { return 0.f; }
        };

        class CircleShape : public Shape {
            float radius;
        public:
            CircleShape(float r = 0.f) : radius(r) {}
            float getRadius() const { return radius; }
        };

        class RectangleShape : public Shape {
        public:
            RectangleShape(const Vector2f&) {}
        };

        class Font {
        public:
            bool loadFromFile(const char*) { return false; }
        };

        class Text {
        public:
            void setFont(const Font&) {}
            void setString(const std::string&) {}
            void setPosition(float, float) {}
            void setCharacterSize(unsigned int) {}
            void setFillColor(const Color&) {}
        };

        class Clock {
        public:
            struct Time { double asSeconds() const { return 0.016; } };
            Time restart() { return Time(); }
        };

        struct Event {
            enum Type { Closed = 0, KeyPressed = 1 };
            Type type;
            struct KeyEvent { int code; } key;
        };

        struct Keyboard {
            enum Key { Left = 0, Right, Up, Down, PageUp, PageDown, Space, Escape };
        };

        class VideoMode {
        public:
            unsigned int width, height;
            VideoMode(unsigned int w = 0, unsigned int h = 0) : width(w), height(h) {}
        };

        class RenderWindow {
            bool open_ = true;
        public:
            RenderWindow(const VideoMode&, const std::string&) {}
            void setFramerateLimit(unsigned int) {}
            Vector2u getSize() const { return Vector2u(1200, 800); }
            bool isOpen() const { return open_; }
            bool pollEvent(Event&) { return false; } // no events in stub
            void clear(const Color&) {}
            void display() {}
            void close() { open_ = false; }
            template<typename T> void draw(const T&) {}
            void draw(const Vertex*, std::size_t, int) {}
        };

        enum PrimitiveType { Lines = 0 };
    }
  #endif
#else
  #include <SFML/Graphics.hpp>
  #include <SFML/Window.hpp>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>
#include <cstring>

// Particle structure representing a DNA configuration
struct Particle {
    float x, y, z;  // 3D position (projected to 2D)
    float fitness;  // Runtime in ms (lower is better)
    int generation; // Generation/step number
    bool is_best;   // Whether this is the best so far
    std::string dna_str; // DNA parameters as string
    
    // Color based on fitness (green = good, red = bad)
    sf::Color getColor() const {
        if (is_best) return sf::Color::Yellow;
        
        // Normalize fitness to 0-1 range (assuming max 1000ms)
        float normalized = std::min(1.0f, fitness / 1000.0f);
        int r = static_cast<int>(255 * normalized);
        int g = static_cast<int>(255 * (1.0f - normalized));
        return sf::Color(r, g, 0);
    }
};

// CSV Reader class
class CSVReader {
public:
    struct CSVRow {
        int generation;
        std::string algo;
        std::string optimizer;
        std::string dna_params;
        double fitness;
        uint64_t comparisons;
        uint64_t swaps;

        CSVRow()
            : generation(0), algo(), optimizer(), dna_params(), fitness(0.0), comparisons(0), swaps(0) {}
    };
    
    std::vector<CSVRow> rows;
    
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open CSV file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        bool first_line = true;
        
        while (std::getline(file, line)) {
            if (first_line) {
                first_line = false;
                continue; // Skip header
            }
            
            if (line.empty()) continue;
            
            CSVRow row;
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            // Parse CSV (handle quoted fields)
            bool in_quotes = false;
            std::string current;
            for (char c : line) {
                if (c == '"') {
                    in_quotes = !in_quotes;
                } else if (c == ',' && !in_quotes) {
                    tokens.push_back(current);
                    current.clear();
                } else {
                    current += c;
                }
            }
            tokens.push_back(current);
            
            if (tokens.size() >= 6) {
                try {
                    row.generation = std::stoi(tokens[0]);
                    row.algo = tokens[1];
                    row.optimizer = tokens[2];
                    row.dna_params = tokens[3];
                    row.fitness = std::stod(tokens[4]);
                    row.comparisons = std::stoull(tokens.size() > 5 ? tokens[5] : "0");
                    row.swaps = std::stoull(tokens.size() > 6 ? tokens[6] : "0");
                    rows.push_back(row);
                } catch (...) {
                    std::cerr << "Warning: Failed to parse line: " << line << std::endl;
                }
            }
        }
        
        file.close();
        std::cout << "Loaded " << rows.size() << " rows from CSV" << std::endl;
        return !rows.empty();
    }
    
    // Convert CSV rows to particles
    std::vector<Particle> toParticles(bool best_only = false) {
        std::vector<Particle> particles;
        if (rows.empty()) return particles;
        
        // Find min/max fitness for normalization
        double min_fitness = rows[0].fitness;
        double max_fitness = rows[0].fitness;
        for (const auto& row : rows) {
            min_fitness = std::min(min_fitness, row.fitness);
            max_fitness = std::max(max_fitness, row.fitness);
        }
        
        // Track best fitness per generation
        std::map<int, double> best_fitness;
        for (const auto& row : rows) {
            if (best_fitness.find(row.generation) == best_fitness.end() || 
                row.fitness < best_fitness[row.generation]) {
                best_fitness[row.generation] = row.fitness;
            }
        }
        
        // Create particles
        for (const auto& row : rows) {
            bool is_best_in_gen = (row.fitness == best_fitness[row.generation]);
            
            if (best_only && !is_best_in_gen) continue;
            
            Particle p;
            p.generation = row.generation;
            p.fitness = static_cast<float>(row.fitness);
            p.is_best = is_best_in_gen;
            p.dna_str = row.dna_params;
            
            // Map DNA parameters to 3D coordinates
            // Use a simple hash/position mapping based on DNA string
            float hash_x = 0.0f, hash_y = 0.0f, hash_z = 0.0f;
            for (size_t i = 0; i < row.dna_params.size(); ++i) {
                hash_x += static_cast<float>(row.dna_params[i]) * (i + 1) * 0.01f;
                hash_y += static_cast<float>(row.dna_params[i]) * (i + 2) * 0.015f;
                hash_z += static_cast<float>(row.dna_params[i]) * (i + 3) * 0.02f;
            }
            
            // Normalize to reasonable range (use fmod for floats)
            p.x = std::fmod(hash_x * 100.0f, 800.0f);
            p.y = std::fmod(hash_y * 100.0f, 600.0f);
            p.z = std::fmod(hash_z * 100.0f, 400.0f);
            
            // Add generation-based offset for time progression
            p.x += row.generation * 10.0f;
            p.y += row.generation * 5.0f;
            
            particles.push_back(p);
        }
        
        return particles;
    }
};

// Particle Renderer class
class ParticleRenderer {
private:
    sf::RenderWindow& window;
    std::vector<Particle> particles;
    int current_generation;
    float speed;
    bool show_all;
    
    // Camera controls
    float camera_angle_x = 0.0f;
    float camera_angle_y = 0.0f;
    float zoom = 1.0f;
    
public:
    ParticleRenderer(sf::RenderWindow& win) : window(win), current_generation(0), speed(1.0f), show_all(true) {}
    
    void setParticles(const std::vector<Particle>& p) {
        particles = p;
        if (!particles.empty()) {
            current_generation = particles[0].generation;
        }
    }
    
    void setSpeed(float s) { speed = s; }
    void setShowAll(bool all) { show_all = all; }
    
    // Project 3D to 2D with simple perspective
    sf::Vector2f project3D(float x, float y, float z) {
        // Simple isometric projection
        float center_x = window.getSize().x / 2.0f;
        float center_y = window.getSize().y / 2.0f;
        
        // Rotate around X and Y axes
        float cos_x = std::cos(camera_angle_x);
        float sin_x = std::sin(camera_angle_x);
        float cos_y = std::cos(camera_angle_y);
        float sin_y = std::sin(camera_angle_y);
        
        // Apply rotation
        float x_rot = x * cos_y - z * sin_y;
        float y_rot = x * sin_x * sin_y + y * cos_x + z * sin_x * cos_y;
        float z_rot = x * cos_x * sin_y - y * sin_x + z * cos_x * cos_y;
        
        // Perspective projection
        float depth = 500.0f + z_rot;
        if (depth <= 0) depth = 0.1f;
        
        float scale = zoom * 500.0f / depth;
        float proj_x = center_x + x_rot * scale;
        float proj_y = center_y + y_rot * scale;
        
        return sf::Vector2f(proj_x, proj_y);
    }
    
    void update(float deltaTime) {
        if (particles.empty()) return;
        
        // Find max generation
        int max_gen = particles[0].generation;
        for (const auto& p : particles) {
            max_gen = std::max(max_gen, p.generation);
        }
        
        // Advance generation based on speed
        static float time_accum = 0.0f;
        time_accum += deltaTime * speed;
        if (time_accum > 0.5f) { // Change generation every 0.5 seconds
            current_generation = std::min(current_generation + 1, max_gen);
            time_accum = 0.0f;
        }
    }
    
    void handleInput(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                camera_angle_y -= 0.1f;
            } else if (event.key.code == sf::Keyboard::Right) {
                camera_angle_y += 0.1f;
            } else if (event.key.code == sf::Keyboard::Up) {
                camera_angle_x -= 0.1f;
            } else if (event.key.code == sf::Keyboard::Down) {
                camera_angle_x += 0.1f;
            } else if (event.key.code == sf::Keyboard::PageUp) {
                zoom *= 1.1f;
            } else if (event.key.code == sf::Keyboard::PageDown) {
                zoom /= 1.1f;
            }
        }
    }
    
    void render() {
        if (particles.empty()) return;
        
        // Draw particles up to current generation
        for (const auto& particle : particles) {
            if (particle.generation > current_generation) continue;
            if (!show_all && !particle.is_best && particle.generation < current_generation) continue;
            
            sf::Vector2f pos = project3D(particle.x, particle.y, particle.z);
            
            // Draw particle
            sf::CircleShape circle(particle.is_best ? 8.0f : 4.0f);
            circle.setPosition(pos.x - circle.getRadius(), pos.y - circle.getRadius());
            circle.setFillColor(particle.getColor());
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(1.0f);
            window.draw(circle);
            
            // Draw trail for best particles
            if (particle.is_best && particle.generation == current_generation) {
                // Find previous best particles
                for (const auto& prev : particles) {
                    if (prev.is_best && prev.generation < particle.generation) {
                        sf::Vector2f prev_pos = project3D(prev.x, prev.y, prev.z);
                        sf::Vertex line[2] = {
                            sf::Vertex(prev_pos, sf::Color(255, 255, 0, 100)),
                            sf::Vertex(pos, sf::Color(255, 255, 0, 200))
                        };
                        window.draw(line, 2, sf::Lines);
                    }
                }
            }
        }
    }
    
    int getCurrentGeneration() const { return current_generation; }
};

// HUD (Heads-Up Display) class
class HUD {
private:
    sf::Font font;
    bool font_loaded;
    CSVReader::CSVRow current_best;
    std::string algo_type;
    std::string optimizer_type;
    int total_generations;
    
    // Simple text rendering without font (fallback)
    void drawSimpleText(sf::RenderWindow& window, const std::string& text, float x, float y, 
                       sf::Color color = sf::Color::White, float size = 14.0f) {
        // Draw simple rectangles representing text (visual placeholder)
        // Each character is approximated by a small rectangle
        float char_width = size * 0.6f;
        float char_height = size;
        
        for (size_t i = 0; i < text.length() && i < 50; ++i) {
            if (text[i] != ' ') {
                sf::RectangleShape char_rect(sf::Vector2f(char_width * 0.8f, char_height * 0.1f));
                char_rect.setPosition(x + i * char_width, y + char_height * 0.4f);
                char_rect.setFillColor(color);
                window.draw(char_rect);
            }
        }
    }
    
public:
    HUD() : font_loaded(false), total_generations(0) {
        // Try to load system font (works on some systems)
        // On Windows, try common font paths
        #ifdef _WIN32
        const char* font_paths[] = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "C:/Windows/Fonts/times.ttf"
        };
        for (const char* path : font_paths) {
            if (font.loadFromFile(path)) {
                font_loaded = true;
                break;
            }
        }
        #else
        // Try Linux/Mac common paths
        const char* font_paths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/Library/Fonts/Arial.ttf"
        };
        for (const char* path : font_paths) {
            if (font.loadFromFile(path)) {
                font_loaded = true;
                break;
            }
        }
        #endif
    }
    
    void setData(const CSVReader& reader, int current_gen) {
        if (reader.rows.empty()) return;
        
        total_generations = reader.rows.back().generation;
        algo_type = reader.rows[0].algo;
        optimizer_type = reader.rows[0].optimizer;
        
        // Find best particle at current generation
        double best_fitness = 1e9;
        current_best = CSVReader::CSVRow();
        for (const auto& row : reader.rows) {
            if (row.generation <= current_gen && row.fitness < best_fitness) {
                best_fitness = row.fitness;
                current_best = row;
            }
        }
    }
    
    void render(sf::RenderWindow& window, int current_gen) {
        const float padding = 10.0f;
        float y_offset = padding + 20.0f;
        
        // Background panel
        sf::RectangleShape panel(sf::Vector2f(350, 280));
        panel.setPosition(padding, padding);
        panel.setFillColor(sf::Color(0, 0, 0, 200));
        panel.setOutlineColor(sf::Color::White);
        panel.setOutlineThickness(2.0f);
        window.draw(panel);
        
        // Helper lambda to draw text
        auto drawText = [&](const std::string& text, float y, sf::Color color = sf::Color::White) {
            if (font_loaded) {
                sf::Text sfml_text;
                sfml_text.setFont(font);
                sfml_text.setString(text);
                sfml_text.setPosition(padding + 15, y);
                sfml_text.setCharacterSize(14);
                sfml_text.setFillColor(color);
                window.draw(sfml_text);
            } else {
                drawSimpleText(window, text, padding + 15, y, color);
            }
        };
        
        // Title
        drawText("=== Evolution Stats ===", y_offset, sf::Color::Cyan);
        y_offset += 25;
        
        // Algorithm info
        drawText("Algorithm: " + algo_type, y_offset);
        y_offset += 20;
        
        drawText("Optimizer: " + optimizer_type, y_offset);
        y_offset += 20;
        
        // Generation info
        std::string gen_str = "Generation: " + std::to_string(current_gen) + " / " + std::to_string(total_generations);
        drawText(gen_str, y_offset, sf::Color::Yellow);
        y_offset += 25;
        
        // Best fitness
        if (current_best.fitness > 0) {
            std::string fitness_str = "Best Fitness: " + 
                std::to_string(static_cast<int>(current_best.fitness * 100) / 100.0) + " ms";
            drawText(fitness_str, y_offset, sf::Color::Green);
            y_offset += 20;
            
            // Best DNA (truncated)
            std::string dna_display = current_best.dna_params;
            if (dna_display.length() > 35) {
                dna_display = dna_display.substr(0, 32) + "...";
            }
            drawText("DNA: " + dna_display, y_offset, sf::Color(200, 200, 255));
            y_offset += 20;
            
            // Metrics
            std::string comp_str = "Comparisons: " + std::to_string(current_best.comparisons);
            drawText(comp_str, y_offset, sf::Color(150, 150, 150));
            y_offset += 20;
            
            std::string swaps_str = "Swaps: " + std::to_string(current_best.swaps);
            drawText(swaps_str, y_offset, sf::Color(150, 150, 150));
        }
        
        // Instructions panel
        float inst_y = window.getSize().y - 140;
        sf::RectangleShape inst_panel(sf::Vector2f(280, 130));
        inst_panel.setPosition(padding, inst_y);
        inst_panel.setFillColor(sf::Color(0, 0, 0, 180));
        inst_panel.setOutlineColor(sf::Color(100, 100, 255));
        inst_panel.setOutlineThickness(1.5f);
        window.draw(inst_panel);
        
        drawText("=== Controls ===", inst_y + 5, sf::Color::Cyan);
        inst_y += 25;
        drawText("Arrows: Rotate view", inst_y);
        inst_y += 18;
        drawText("PgUp/PgDn: Zoom", inst_y);
        inst_y += 18;
        drawText("Space: Pause/Resume", inst_y);
        inst_y += 18;
        drawText("ESC: Exit", inst_y);
    }
};

// Main function
int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string csv_file = "data/logs/run_ga_qs.csv";
    float speed = 1.0f;
    bool best_only = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--in=") == 0) {
            csv_file = arg.substr(5);
        } else if (arg.find("--speed=") == 0) {
            speed = std::stof(arg.substr(8));
        } else if (arg == "--mode=best-only") {
            best_only = true;
        }
    }
    
    // Load CSV
    CSVReader reader;
    if (!reader.load(csv_file)) {
        std::cerr << "Failed to load CSV file. Exiting." << std::endl;
        return 1;
    }
    
    // Convert to particles
    std::vector<Particle> particles = reader.toParticles(best_only);
    if (particles.empty()) {
        std::cerr << "No particles to visualize. Exiting." << std::endl;
        return 1;
    }
    
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Algorithm Evolution Visualization");
    window.setFramerateLimit(60);
    
    // Create renderer and HUD
    ParticleRenderer renderer(window);
    renderer.setParticles(particles);
    renderer.setSpeed(speed);
    renderer.setShowAll(!best_only);
    
    HUD hud;
    
    // Main loop
    sf::Clock clock;
    bool paused = false;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                } else {
                    renderer.handleInput(event);
                }
            }
        }
        
        // Update
        if (!paused) {
            float deltaTime = clock.restart().asSeconds();
            renderer.update(deltaTime);
        } else {
            clock.restart(); // Don't accumulate time when paused
        }
        
        hud.setData(reader, renderer.getCurrentGeneration());
        
        // Render
        window.clear(sf::Color(20, 20, 30));
        
        renderer.render();
        hud.render(window, renderer.getCurrentGeneration());
        
        window.display();
    }
    
    return 0;
}

#if defined(__has_include)
  #if __has_include(<SFML/Graphics.hpp>) && __has_include(<SFML/Window.hpp>)
    #include <SFML/Graphics.hpp>
    #include <SFML/Window.hpp>
  #else
    // SFML headers not found — provide minimal compile-time stubs so the file
    // can be parsed/compiled in environments without SFML installed.
    #include <cstdint>
    #include <cstddef>
    #include <string>
    #include <vector>

    namespace sf {
        struct Color { uint8_t r,g,b,a; Color(uint8_t rr=0,uint8_t gg=0,uint8_t bb=0,uint8_t aa=255):r(rr),g(gg),b(bb),a(aa){} };
        struct Vector2f { float x,y; Vector2f(float xx=0,float yy=0):x(xx),y(yy){} };
        struct Vector2u { unsigned int x,y; Vector2u(unsigned int xx=0,unsigned int yy=0):x(xx),y(yy){} };

        struct Vertex { Vector2f position; Color color; Vertex() : position(), color() {} Vertex(const Vector2f& p, const Color& c) : position(p), color(c) {} };

        class Shape {
        public:
            virtual ~Shape() {}
            void setPosition(float, float) {}
            void setFillColor(const Color&) {}
            void setOutlineColor(const Color&) {}
            void setOutlineThickness(float) {}
            float getRadius() const { return 0.f; }
        };

        class CircleShape : public Shape {
            float radius;
        public:
            CircleShape(float r = 0.f) : radius(r) {}
            float getRadius() const { return radius; }
        };

        class RectangleShape : public Shape {
        public:
            RectangleShape(const Vector2f&) {}
        };

        class Font {
        public:
            bool loadFromFile(const char*) { return false; }
        };

        class Text {
        public:
            void setFont(const Font&) {}
            void setString(const std::string&) {}
            void setPosition(float, float) {}
            void setCharacterSize(unsigned int) {}
            void setFillColor(const Color&) {}
        };

        class Clock {
        public:
            struct Time { double asSeconds() const { return 0.016; } };
            Time restart() { return Time(); }
        };

        struct Event {
            enum Type { Closed = 0, KeyPressed = 1 };
            Type type;
            struct KeyEvent { int code; } key;
        };

        struct Keyboard {
            enum Key { Left = 0, Right, Up, Down, PageUp, PageDown, Space, Escape };
        };

        class VideoMode {
        public:
            unsigned int width, height;
            VideoMode(unsigned int w = 0, unsigned int h = 0) : width(w), height(h) {}
        };

        class RenderWindow {
            bool open_ = true;
        public:
            RenderWindow(const VideoMode&, const std::string&) {}
            void setFramerateLimit(unsigned int) {}
            Vector2u getSize() const { return Vector2u(1200, 800); }
            bool isOpen() const { return open_; }
            bool pollEvent(Event&) { return false; } // no events in stub
            void clear(const Color&) {}
            void display() {}
            void close() { open_ = false; }
            template<typename T> void draw(const T&) {}
            void draw(const Vertex*, std::size_t, int) {}
        };

        enum PrimitiveType { Lines = 0 };
    }
  #endif
#else
  #include <SFML/Graphics.hpp>
  #include <SFML/Window.hpp>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>
#include <cstring>

// Particle structure representing a DNA configuration
struct Particle {
    float x, y, z;  // 3D position (projected to 2D)
    float fitness;  // Runtime in ms (lower is better)
    int generation; // Generation/step number
    bool is_best;   // Whether this is the best so far
    std::string dna_str; // DNA parameters as string
    
    // Color based on fitness (green = good, red = bad)
    sf::Color getColor() const {
        if (is_best) return sf::Color::Yellow;
        
        // Normalize fitness to 0-1 range (assuming max 1000ms)
        float normalized = std::min(1.0f, fitness / 1000.0f);
        int r = static_cast<int>(255 * normalized);
        int g = static_cast<int>(255 * (1.0f - normalized));
        return sf::Color(r, g, 0);
    }
};

// CSV Reader class
class CSVReader {
public:
    struct CSVRow {
        int generation;
        std::string algo;
        std::string optimizer;
        std::string dna_params;
        double fitness;
        uint64_t comparisons;
        uint64_t swaps;

        CSVRow()
            : generation(0), algo(), optimizer(), dna_params(), fitness(0.0), comparisons(0), swaps(0) {}
    };
    
    std::vector<CSVRow> rows;
    
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open CSV file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        bool first_line = true;
        
        while (std::getline(file, line)) {
            if (first_line) {
                first_line = false;
                continue; // Skip header
            }
            
            if (line.empty()) continue;
            
            CSVRow row;
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            // Parse CSV (handle quoted fields)
            bool in_quotes = false;
            std::string current;
            for (char c : line) {
                if (c == '"') {
                    in_quotes = !in_quotes;
                } else if (c == ',' && !in_quotes) {
                    tokens.push_back(current);
                    current.clear();
                } else {
                    current += c;
                }
            }
            tokens.push_back(current);
            
            if (tokens.size() >= 6) {
                try {
                    row.generation = std::stoi(tokens[0]);
                    row.algo = tokens[1];
                    row.optimizer = tokens[2];
                    row.dna_params = tokens[3];
                    row.fitness = std::stod(tokens[4]);
                    row.comparisons = std::stoull(tokens.size() > 5 ? tokens[5] : "0");
                    row.swaps = std::stoull(tokens.size() > 6 ? tokens[6] : "0");
                    rows.push_back(row);
                } catch (...) {
                    std::cerr << "Warning: Failed to parse line: " << line << std::endl;
                }
            }
        }
        
        file.close();
        std::cout << "Loaded " << rows.size() << " rows from CSV" << std::endl;
        return !rows.empty();
    }
    
    // Convert CSV rows to particles
    std::vector<Particle> toParticles(bool best_only = false) {
        std::vector<Particle> particles;
        if (rows.empty()) return particles;
        
        // Find min/max fitness for normalization
        double min_fitness = rows[0].fitness;
        double max_fitness = rows[0].fitness;
        for (const auto& row : rows) {
            min_fitness = std::min(min_fitness, row.fitness);
            max_fitness = std::max(max_fitness, row.fitness);
        }
        
        // Track best fitness per generation
        std::map<int, double> best_fitness;
        for (const auto& row : rows) {
            if (best_fitness.find(row.generation) == best_fitness.end() || 
                row.fitness < best_fitness[row.generation]) {
                best_fitness[row.generation] = row.fitness;
            }
        }
        
        // Create particles
        for (const auto& row : rows) {
            bool is_best_in_gen = (row.fitness == best_fitness[row.generation]);
            
            if (best_only && !is_best_in_gen) continue;
            
            Particle p;
            p.generation = row.generation;
            p.fitness = static_cast<float>(row.fitness);
            p.is_best = is_best_in_gen;
            p.dna_str = row.dna_params;
            
            // Map DNA parameters to 3D coordinates
            // Use a simple hash/position mapping based on DNA string
            float hash_x = 0.0f, hash_y = 0.0f, hash_z = 0.0f;
            for (size_t i = 0; i < row.dna_params.size(); ++i) {
                hash_x += static_cast<float>(row.dna_params[i]) * (i + 1) * 0.01f;
                hash_y += static_cast<float>(row.dna_params[i]) * (i + 2) * 0.015f;
                hash_z += static_cast<float>(row.dna_params[i]) * (i + 3) * 0.02f;
            }
            
            // Normalize to reasonable range (use fmod for floats)
            p.x = std::fmod(hash_x * 100.0f, 800.0f);
            p.y = std::fmod(hash_y * 100.0f, 600.0f);
            p.z = std::fmod(hash_z * 100.0f, 400.0f);
            
            // Add generation-based offset for time progression
            p.x += row.generation * 10.0f;
            p.y += row.generation * 5.0f;
            
            particles.push_back(p);
        }
        
        return particles;
    }
};

// Particle Renderer class
class ParticleRenderer {
private:
    sf::RenderWindow& window;
    std::vector<Particle> particles;
    int current_generation;
    float speed;
    bool show_all;
    
    // Camera controls
    float camera_angle_x = 0.0f;
    float camera_angle_y = 0.0f;
    float zoom = 1.0f;
    
public:
    ParticleRenderer(sf::RenderWindow& win) : window(win), current_generation(0), speed(1.0f), show_all(true) {}
    
    void setParticles(const std::vector<Particle>& p) {
        particles = p;
        if (!particles.empty()) {
            current_generation = particles[0].generation;
        }
    }
    
    void setSpeed(float s) { speed = s; }
    void setShowAll(bool all) { show_all = all; }
    
    // Project 3D to 2D with simple perspective
    sf::Vector2f project3D(float x, float y, float z) {
        // Simple isometric projection
        float center_x = window.getSize().x / 2.0f;
        float center_y = window.getSize().y / 2.0f;
        
        // Rotate around X and Y axes
        float cos_x = std::cos(camera_angle_x);
        float sin_x = std::sin(camera_angle_x);
        float cos_y = std::cos(camera_angle_y);
        float sin_y = std::sin(camera_angle_y);
        
        // Apply rotation
        float x_rot = x * cos_y - z * sin_y;
        float y_rot = x * sin_x * sin_y + y * cos_x + z * sin_x * cos_y;
        float z_rot = x * cos_x * sin_y - y * sin_x + z * cos_x * cos_y;
        
        // Perspective projection
        float depth = 500.0f + z_rot;
        if (depth <= 0) depth = 0.1f;
        
        float scale = zoom * 500.0f / depth;
        float proj_x = center_x + x_rot * scale;
        float proj_y = center_y + y_rot * scale;
        
        return sf::Vector2f(proj_x, proj_y);
    }
    
    void update(float deltaTime) {
        if (particles.empty()) return;
        
        // Find max generation
        int max_gen = particles[0].generation;
        for (const auto& p : particles) {
            max_gen = std::max(max_gen, p.generation);
        }
        
        // Advance generation based on speed
        static float time_accum = 0.0f;
        time_accum += deltaTime * speed;
        if (time_accum > 0.5f) { // Change generation every 0.5 seconds
            current_generation = std::min(current_generation + 1, max_gen);
            time_accum = 0.0f;
        }
    }
    
    void handleInput(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                camera_angle_y -= 0.1f;
            } else if (event.key.code == sf::Keyboard::Right) {
                camera_angle_y += 0.1f;
            } else if (event.key.code == sf::Keyboard::Up) {
                camera_angle_x -= 0.1f;
            } else if (event.key.code == sf::Keyboard::Down) {
                camera_angle_x += 0.1f;
            } else if (event.key.code == sf::Keyboard::PageUp) {
                zoom *= 1.1f;
            } else if (event.key.code == sf::Keyboard::PageDown) {
                zoom /= 1.1f;
            }
        }
    }
    
    void render() {
        if (particles.empty()) return;
        
        // Draw particles up to current generation
        for (const auto& particle : particles) {
            if (particle.generation > current_generation) continue;
            if (!show_all && !particle.is_best && particle.generation < current_generation) continue;
            
            sf::Vector2f pos = project3D(particle.x, particle.y, particle.z);
            
            // Draw particle
            sf::CircleShape circle(particle.is_best ? 8.0f : 4.0f);
            circle.setPosition(pos.x - circle.getRadius(), pos.y - circle.getRadius());
            circle.setFillColor(particle.getColor());
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(1.0f);
            window.draw(circle);
            
            // Draw trail for best particles
            if (particle.is_best && particle.generation == current_generation) {
                // Find previous best particles
                for (const auto& prev : particles) {
                    if (prev.is_best && prev.generation < particle.generation) {
                        sf::Vector2f prev_pos = project3D(prev.x, prev.y, prev.z);
                        sf::Vertex line[2] = {
                            sf::Vertex(prev_pos, sf::Color(255, 255, 0, 100)),
                            sf::Vertex(pos, sf::Color(255, 255, 0, 200))
                        };
                        window.draw(line, 2, sf::Lines);
                    }
                }
            }
        }
    }
    
    int getCurrentGeneration() const { return current_generation; }
};

// HUD (Heads-Up Display) class
class HUD {
private:
    sf::Font font;
    bool font_loaded;
    CSVReader::CSVRow current_best;
    std::string algo_type;
    std::string optimizer_type;
    int total_generations;
    
    // Simple text rendering without font (fallback)
    void drawSimpleText(sf::RenderWindow& window, const std::string& text, float x, float y, 
                       sf::Color color = sf::Color::White, float size = 14.0f) {
        // Draw simple rectangles representing text (visual placeholder)
        // Each character is approximated by a small rectangle
        float char_width = size * 0.6f;
        float char_height = size;
        
        for (size_t i = 0; i < text.length() && i < 50; ++i) {
            if (text[i] != ' ') {
                sf::RectangleShape char_rect(sf::Vector2f(char_width * 0.8f, char_height * 0.1f));
                char_rect.setPosition(x + i * char_width, y + char_height * 0.4f);
                char_rect.setFillColor(color);
                window.draw(char_rect);
            }
        }
    }
    
public:
    HUD() : font_loaded(false), total_generations(0) {
        // Try to load system font (works on some systems)
        // On Windows, try common font paths
        #ifdef _WIN32
        const char* font_paths[] = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "C:/Windows/Fonts/times.ttf"
        };
        for (const char* path : font_paths) {
            if (font.loadFromFile(path)) {
                font_loaded = true;
                break;
            }
        }
        #else
        // Try Linux/Mac common paths
        const char* font_paths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/Library/Fonts/Arial.ttf"
        };
        for (const char* path : font_paths) {
            if (font.loadFromFile(path)) {
                font_loaded = true;
                break;
            }
        }
        #endif
    }
    
    void setData(const CSVReader& reader, int current_gen) {
        if (reader.rows.empty()) return;
        
        total_generations = reader.rows.back().generation;
        algo_type = reader.rows[0].algo;
        optimizer_type = reader.rows[0].optimizer;
        
        // Find best particle at current generation
        double best_fitness = 1e9;
        current_best = CSVReader::CSVRow();
        for (const auto& row : reader.rows) {
            if (row.generation <= current_gen && row.fitness < best_fitness) {
                best_fitness = row.fitness;
                current_best = row;
            }
        }
    }
    
    void render(sf::RenderWindow& window, int current_gen) {
        const float padding = 10.0f;
        float y_offset = padding + 20.0f;
        
        // Background panel
        sf::RectangleShape panel(sf::Vector2f(350, 280));
        panel.setPosition(padding, padding);
        panel.setFillColor(sf::Color(0, 0, 0, 200));
        panel.setOutlineColor(sf::Color::White);
        panel.setOutlineThickness(2.0f);
        window.draw(panel);
        
        // Helper lambda to draw text
        auto drawText = [&](const std::string& text, float y, sf::Color color = sf::Color::White) {
            if (font_loaded) {
                sf::Text sfml_text;
                sfml_text.setFont(font);
                sfml_text.setString(text);
                sfml_text.setPosition(padding + 15, y);
                sfml_text.setCharacterSize(14);
                sfml_text.setFillColor(color);
                window.draw(sfml_text);
            } else {
                drawSimpleText(window, text, padding + 15, y, color);
            }
        };
        
        // Title
        drawText("=== Evolution Stats ===", y_offset, sf::Color::Cyan);
        y_offset += 25;
        
        // Algorithm info
        drawText("Algorithm: " + algo_type, y_offset);
        y_offset += 20;
        
        drawText("Optimizer: " + optimizer_type, y_offset);
        y_offset += 20;
        
        // Generation info
        std::string gen_str = "Generation: " + std::to_string(current_gen) + " / " + std::to_string(total_generations);
        drawText(gen_str, y_offset, sf::Color::Yellow);
        y_offset += 25;
        
        // Best fitness
        if (current_best.fitness > 0) {
            std::string fitness_str = "Best Fitness: " + 
                std::to_string(static_cast<int>(current_best.fitness * 100) / 100.0) + " ms";
            drawText(fitness_str, y_offset, sf::Color::Green);
            y_offset += 20;
            
            // Best DNA (truncated)
            std::string dna_display = current_best.dna_params;
            if (dna_display.length() > 35) {
                dna_display = dna_display.substr(0, 32) + "...";
            }
            drawText("DNA: " + dna_display, y_offset, sf::Color(200, 200, 255));
            y_offset += 20;
            
            // Metrics
            std::string comp_str = "Comparisons: " + std::to_string(current_best.comparisons);
            drawText(comp_str, y_offset, sf::Color(150, 150, 150));
            y_offset += 20;
            
            std::string swaps_str = "Swaps: " + std::to_string(current_best.swaps);
            drawText(swaps_str, y_offset, sf::Color(150, 150, 150));
        }
        
        // Instructions panel
        float inst_y = window.getSize().y - 140;
        sf::RectangleShape inst_panel(sf::Vector2f(280, 130));
        inst_panel.setPosition(padding, inst_y);
        inst_panel.setFillColor(sf::Color(0, 0, 0, 180));
        inst_panel.setOutlineColor(sf::Color(100, 100, 255));
        inst_panel.setOutlineThickness(1.5f);
        window.draw(inst_panel);
        
        drawText("=== Controls ===", inst_y + 5, sf::Color::Cyan);
        inst_y += 25;
        drawText("Arrows: Rotate view", inst_y);
        inst_y += 18;
        drawText("PgUp/PgDn: Zoom", inst_y);
        inst_y += 18;
        drawText("Space: Pause/Resume", inst_y);
        inst_y += 18;
        drawText("ESC: Exit", inst_y);
    }
};

// Main function
int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string csv_file = "data/logs/run_ga_qs.csv";
    float speed = 1.0f;
    bool best_only = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--in=") == 0) {
            csv_file = arg.substr(5);
        } else if (arg.find("--speed=") == 0) {
            speed = std::stof(arg.substr(8));
        } else if (arg == "--mode=best-only") {
            best_only = true;
        }
    }
    
    // Load CSV
    CSVReader reader;
    if (!reader.load(csv_file)) {
        std::cerr << "Failed to load CSV file. Exiting." << std::endl;
        return 1;
    }
    
    // Convert to particles
    std::vector<Particle> particles = reader.toParticles(best_only);
    if (particles.empty()) {
        std::cerr << "No particles to visualize. Exiting." << std::endl;
        return 1;
    }
    
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Algorithm Evolution Visualization");
    window.setFramerateLimit(60);
    
    // Create renderer and HUD
    ParticleRenderer renderer(window);
    renderer.setParticles(particles);
    renderer.setSpeed(speed);
    renderer.setShowAll(!best_only);
    
    HUD hud;
    
    // Main loop
    sf::Clock clock;
    bool paused = false;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                } else {
                    renderer.handleInput(event);
                }
            }
        }
        
        // Update
        if (!paused) {
            float deltaTime = clock.restart().asSeconds();
            renderer.update(deltaTime);
        } else {
            clock.restart(); // Don't accumulate time when paused
        }
        
        hud.setData(reader, renderer.getCurrentGeneration());
        
        // Render
        window.clear(sf::Color(20, 20, 30));
        
        renderer.render();
        hud.render(window, renderer.getCurrentGeneration());
        
        window.display();
    }
    
    return 0;
}
