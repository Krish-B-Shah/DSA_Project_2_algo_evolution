// viz_utils.cpp - Optional helper utilities for visualization
// Separates drawing, CSV parsing, and math utilities from viz_sfml.cpp

#if defined(__has_include)
  #if __has_include(<SFML/Graphics.hpp>) && __has_include(<SFML/Window.hpp>)
    #include <SFML/Graphics.hpp>
    #include <SFML/Window.hpp>
  #else
    // SFML headers not found — provide minimal compile-time stubs
    #include <cstdint>
    #include <cstddef>
    #include <string>
    #include <vector>

    namespace sf {
        struct Color { uint8_t r,g,b,a; Color(uint8_t rr=0,uint8_t gg=0,uint8_t bb=0,uint8_t aa=255):r(rr),g(gg),b(bb),a(aa){} };
        struct Vector2f { float x,y; Vector2f(float xx=0,float yy=0):x(xx),y(yy){} };
        struct Vector2u { unsigned int x,y; Vector2u(unsigned int xx=0,unsigned int yy=0):x(xx),y(yy){} };
    }
  #endif
#else
  #include <SFML/Graphics.hpp>
  #include <SFML/Window.hpp>
#endif

#include <cmath>
#include <algorithm>
#include <string>

namespace VizUtils {

// ============================================================================
// Perspective Projection & Math Utilities
// ============================================================================

/**
 * Calculate perspective scale factor based on depth
 * @param depth Distance from camera (z-coordinate after rotation)
 * @param focal_length Focal length for perspective (default: 500.0f)
 * @param zoom Zoom multiplier (default: 1.0f)
 * @return Scale factor for perspective projection
 */
inline float perspectiveScale(float depth, float focal_length = 500.0f, float zoom = 1.0f) {
    if (depth <= 0) depth = 0.1f; // Avoid division by zero
    return zoom * focal_length / depth;
}

/**
 * Project 3D coordinates to 2D screen coordinates with perspective
 * @param x, y, z 3D coordinates
 * @param center_x, center_y Screen center coordinates
 * @param camera_angle_x Rotation around X axis (pitch)
 * @param camera_angle_y Rotation around Y axis (yaw)
 * @param zoom Zoom level
 * @param focal_length Focal length for perspective
 * @return 2D projected coordinates
 */
sf::Vector2f project3D(
    float x, float y, float z,
    float center_x, float center_y,
    float camera_angle_x = 0.0f,
    float camera_angle_y = 0.0f,
    float zoom = 1.0f,
    float focal_length = 500.0f
) {
    // Calculate rotation matrices
    float cos_x = std::cos(camera_angle_x);
    float sin_x = std::sin(camera_angle_x);
    float cos_y = std::cos(camera_angle_y);
    float sin_y = std::sin(camera_angle_y);
    
    // Apply rotation around Y axis (yaw)
    float x_rot = x * cos_y - z * sin_y;
    float z_rot_y = x * sin_y + z * cos_y;
    
    // Apply rotation around X axis (pitch)
    float y_rot = x_rot * sin_x * sin_y + y * cos_x + z_rot_y * sin_x * cos_y;
    float z_rot = x_rot * cos_x * sin_y - y * sin_x + z_rot_y * cos_x * cos_y;
    
    // Perspective projection
    float depth = focal_length + z_rot;
    float scale = perspectiveScale(depth, focal_length, zoom);
    
    float proj_x = center_x + x_rot * scale;
    float proj_y = center_y + y_rot * scale;
    
    return sf::Vector2f(proj_x, proj_y);
}

/**
 * Rotate 3D point around X and Y axes
 * @param x, y, z Input coordinates
 * @param angle_x Rotation around X axis
 * @param angle_y Rotation around Y axis
 * @return Rotated coordinates (x_rot, y_rot, z_rot)
 */
struct Rotated3D {
    float x, y, z;
};

inline Rotated3D rotate3D(float x, float y, float z, float angle_x, float angle_y) {
    float cos_x = std::cos(angle_x);
    float sin_x = std::sin(angle_x);
    float cos_y = std::cos(angle_y);
    float sin_y = std::sin(angle_y);
    
    float x_rot = x * cos_y - z * sin_y;
    float y_rot = x * sin_x * sin_y + y * cos_x + z * sin_x * cos_y;
    float z_rot = x * cos_x * sin_y - y * sin_x + z * cos_x * cos_y;
    
    return {x_rot, y_rot, z_rot};
}

// ============================================================================
// Color Mapping Utilities
// ============================================================================

/**
 * Map fitness value to color (green = good/low, red = bad/high)
 * @param fitness Fitness value (lower is better)
 * @param max_fitness Maximum fitness for normalization (default: 1000.0f)
 * @param is_best Whether this is the best particle (returns yellow)
 * @return Color based on fitness
 */
sf::Color fitnessToColor(float fitness, float max_fitness = 1000.0f, bool is_best = false) {
    if (is_best) {
        return sf::Color::Yellow;
    }
    
    // Normalize fitness to 0-1 range
    float normalized = std::min(1.0f, std::max(0.0f, fitness / max_fitness));
    
    // Interpolate: green (0) -> yellow (0.5) -> red (1)
    int r = static_cast<int>(255 * normalized);
    int g = static_cast<int>(255 * (1.0f - normalized));
    int b = 0;
    
    return sf::Color(r, g, b);
}

/**
 * Map normalized value (0-1) to color gradient
 * @param value Normalized value [0, 1]
 * @param color_scheme Color scheme type: 0=red-green, 1=blue-red, 2=rainbow
 * @return Color based on value
 */
sf::Color valueToColor(float value, int color_scheme = 0) {
    value = std::min(1.0f, std::max(0.0f, value));
    
    switch (color_scheme) {
        case 0: // Red to Green (default)
            return sf::Color(
                static_cast<uint8_t>(255 * value),
                static_cast<uint8_t>(255 * (1.0f - value)),
                0
            );
        
        case 1: // Blue to Red
            return sf::Color(
                static_cast<uint8_t>(255 * value),
                0,
                static_cast<uint8_t>(255 * (1.0f - value))
            );
        
        case 2: // Rainbow (HSV-like)
        {
            float h = value * 6.0f; // Hue in [0, 6)
            float s = 1.0f;
            float v = 1.0f;
            
            int i = static_cast<int>(h);
            float f = h - i;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));
            
            float r, g, b;
            switch (i % 6) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
                default: r = g = b = 0; break;
            }
            
            return sf::Color(
                static_cast<uint8_t>(255 * r),
                static_cast<uint8_t>(255 * g),
                static_cast<uint8_t>(255 * b)
            );
        }
        
        default:
            return sf::Color::White;
    }
}

// ============================================================================
// CSV Parsing Utilities
// ============================================================================

/**
 * Parse a single CSV line, handling quoted fields
 * @param line Input line string
 * @return Vector of token strings
 */
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> tokens;
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
    tokens.push_back(current); // Add last token
    
    return tokens;
}

/**
 * Trim whitespace from string
 */
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * Parse CSV token to integer with error handling
 */
inline int parseCSVInt(const std::string& token, int default_value = 0) {
    try {
        return std::stoi(trim(token));
    } catch (...) {
        return default_value;
    }
}

/**
 * Parse CSV token to double with error handling
 */
inline double parseCSVDouble(const std::string& token, double default_value = 0.0) {
    try {
        return std::stod(trim(token));
    } catch (...) {
        return default_value;
    }
}

/**
 * Parse CSV token to unsigned long long with error handling
 */
inline uint64_t parseCSVUInt64(const std::string& token, uint64_t default_value = 0) {
    try {
        return std::stoull(trim(token));
    } catch (...) {
        return default_value;
    }
}

// ============================================================================
// Coordinate Mapping Utilities
// ============================================================================

/**
 * Map DNA string to 3D coordinates using hash function
 * @param dna_str DNA parameter string
 * @param generation Generation number (for time offset)
 * @param bounds_x, bounds_y, bounds_z Coordinate bounds
 * @return 3D coordinates (x, y, z)
 */
struct Coord3D {
    float x, y, z;
};

Coord3D dnaToCoordinates(
    const std::string& dna_str,
    int generation = 0,
    float bounds_x = 800.0f,
    float bounds_y = 600.0f,
    float bounds_z = 400.0f
) {
    // Hash-based coordinate mapping
    float hash_x = 0.0f, hash_y = 0.0f, hash_z = 0.0f;
    
    for (size_t i = 0; i < dna_str.size(); ++i) {
        float char_val = static_cast<float>(dna_str[i]);
        hash_x += char_val * (i + 1) * 0.01f;
        hash_y += char_val * (i + 2) * 0.015f;
        hash_z += char_val * (i + 3) * 0.02f;
    }
    
    // Normalize to bounds
    float x = std::fmod(hash_x * 100.0f, bounds_x);
    float y = std::fmod(hash_y * 100.0f, bounds_y);
    float z = std::fmod(hash_z * 100.0f, bounds_z);
    
    // Add generation-based offset for time progression
    x += generation * 10.0f;
    y += generation * 5.0f;
    
    return {x, y, z};
}

/**
 * Normalize value from [min, max] to [0, 1]
 */
inline float normalize(float value, float min_val, float max_val) {
    if (max_val == min_val) return 0.5f;
    return (value - min_val) / (max_val - min_val);
}

/**
 * Clamp value to range [min, max]
 */
inline float clamp(float value, float min_val, float max_val) {
    return std::min(max_val, std::max(min_val, value));
}

} // namespace VizUtils

