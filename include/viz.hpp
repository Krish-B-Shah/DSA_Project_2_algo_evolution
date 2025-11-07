#pragma once

#include <string>
#include <vector>
// Include lightweight SFML color definition to allow value member usage
#include <SFML/Graphics/Color.hpp>

// Forward declarations to avoid heavy SFML includes in headers
namespace sf {
	class RenderWindow;
}

namespace viz {

struct Particle {
	float x;
	float y;
	float z;
	sf::Color color;
	float size;
};

// Loads particles from a file at `path`.
// The concrete format is defined by the implementation.
std::vector<Particle> loadParticles(const std::string& path);

// Draws the given particles to the provided SFML render window.
void drawParticles(sf::RenderWindow& window, const std::vector<Particle>& particles);

} // namespace viz


