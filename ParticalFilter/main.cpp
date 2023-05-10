//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include <math.h>
#include <ctime>
#include <iostream>
#include <random>
#include "usefullFunctions.h"

sf::RenderWindow window(sf::VideoMode(1200, 900), "Partical Filter");
const int terrainWidth = window.getSize().x;
const double accuracy = 0.9;
const double probOfAction = 0.8;
const double probOfReversedAction = 0.2;


bool stopParam = false;

sf::Font font;


const int shifting = 100;
double terrain(int x) {
    //shifting

    x -= shifting;
    // Изменение этих параметров приведет к различным горам и долинам
    double amplitude1 = 70;  // Амплитуда первой синусоиды
    double frequency1 = 0.04; // Частота первой синусоиды
    double amplitude2 = 40;  // Амплитуда второй синусоиды
    double frequency2 = 0.04; // Частота второй синусоиды
    double amplitude3 = 90;  // Амплитуда второй синусоиды
    double frequency3 = 0.1; // Частота второй синусоиды

    if(x > terrainWidth){
        return -1000;
    }
    if(x > 617-shifting && x < 930-shifting){
        return 196;
    }
    return amplitude1 * sin(frequency1 * x) + amplitude2 * sin(frequency2 * x) + amplitude3 * sin(frequency3 * x);
}

int getAgentPosition(int terrainWidth) {
    srand(time(0)); // Инициализация генератора случайных чисел
    return rand() % (terrainWidth + 1);
    //return 300;
    //return terrainWidth-1;
}

int whereIsAgent(double accuracy, std::vector<double> probabilities){
    int count = 0;
    for(auto const &val : probabilities){
        if(val >= accuracy){
            std::cout << "Agent's position is: " << count << std::endl;
            std::cout << "Probability of thish position is: " << val << std::endl;
            return count;
        }
        count++;
    }
    if(count == 0){
        std::cout << "It's unpossible to find agent's position\n";
        return -1;
    }
    return -1;
}


void updateProbabilities(double agentPosition, std::vector<double>& probabilities){
    
    double agentDistToTerrain = terrain(agentPosition);
    double epsilon = 5;
    double additionalProb = 0;
    int numOfleftedProb = 0;
    double sumOfDeletedProb = 0;
    for(int x = 0; x < probabilities.size(); x++){
        if(probabilities[x] != 0){
            double currentDist = terrain(x);
            if(abs(currentDist - agentDistToTerrain) > epsilon){
                sumOfDeletedProb += probabilities[x];
                probabilities[x] = 0;
            } else {
                numOfleftedProb++;
            }
        }
    }
    double sumOfChances = 0;
    for (int i{}; i < probabilities.size(); ++i)
        sumOfChances += probabilities[i];
    for (int i{}; i < probabilities.size(); ++i)
        probabilities[i] = probabilities[i] / sumOfChances;
}


void moveProbabilities(std::vector<double> &probabilities){
    std::vector<double> new_probabilities(probabilities.size(), 0);
    double sum = 0;
    for(auto const &val : probabilities){
        sum += val;
    }
    if(sum>1.1)
        std::cout << "ERROR prob > 1 " << sum << " \n\n\n";
    
    for(int x = 0; x < probabilities.size(); x++){
        if(x > 0){
            new_probabilities[x] += probabilities[x - 1] * probOfAction;
        }
        if(x < probabilities.size() - 1){
            new_probabilities[x] += probabilities[x + 1] * probOfReversedAction;
        }
        // If agent is at the boundary, it stays in place with probOfAction and the probability of moving into the wall
        if (x == 0) {
            new_probabilities[x] += probabilities[x] * (probOfAction + probOfReversedAction);
        } else if (x == probabilities.size() - 1) {
            new_probabilities[x] += probabilities[x] * (probOfAction + probOfAction);
        } else {
            new_probabilities[x] += probabilities[x] * (1 - probOfAction - probOfReversedAction);
        }
    }
    
    probabilities = new_probabilities;
}






int moveAgent(int &agentPosition, std::vector<double> &probabilities){
    moveProbabilities(probabilities);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double randNum = dis(gen);
    
    if(randNum < probOfReversedAction && agentPosition > 0){
        if(agentPosition-1 < 0){
            return agentPosition;
        }
        return --agentPosition;
    } else {
        if(agentPosition+1 == terrainWidth){
            return agentPosition;
        }
        return ++agentPosition;
    }
}

void updateProbabilityGraph(std::vector<double>& probabilities, sf::VertexArray& probabilityGraph) {
    for (int x = 0; x < probabilities.size(); ++x){
        double y = probabilities[x] * 1000; // Умножьте на 500, чтобы увеличить видимость. Вы можете настроить это значение.
        probabilityGraph[x].position = sf::Vector2f(x, -y+360);
        probabilityGraph[x].color = sf::Color::Red;

    }
}


void moveAction(int currentPos, std::vector<double> &probabilities, int agentPosition, sf::RectangleShape &agentLine, sf::Sprite &agentSprite, sf::VertexArray &probabilityGraph){
    
    agentLine.setPosition(currentPos, 420);
    agentSprite.setPosition(currentPos-agentSprite.getGlobalBounds().width/2, 400);
    updateProbabilities(agentPosition, probabilities);
    updateProbabilityGraph(probabilities, probabilityGraph);
    
}

void callWinWindow(int foundPosition, int agentPosition){
    
    //показать окно срезультатами и преостановить другие действия
    sf::RenderWindow agentWindow(sf::VideoMode(500, 200), "Agent Position");
    sf::Text agentPositionText, header, realAgentPositionText;
    agentPositionText.setFont(font); header.setFont(font); realAgentPositionText.setFont(font);
    agentPositionText.setCharacterSize(24); header.setCharacterSize(24); realAgentPositionText.setCharacterSize(24);
    agentPositionText.setFillColor(sf::Color::Black); header.setFillColor(sf::Color::Black); realAgentPositionText.setFillColor(sf::Color::Black);
    agentPositionText.setString("Found agent's position: " + std::to_string(foundPosition) + " +- 1 pixel"); header.setString("Agent's position has been found!");
    realAgentPositionText.setString("Real agent's position: " + std::to_string(agentPosition));
    agentPositionText.setPosition(getCenterPosition(agentPositionText, agentWindow).x, getCenterPosition(agentPositionText, agentWindow).y+15);
    header.setPosition(getCenterPosition(header, agentWindow).x, getCenterPosition(header, agentWindow).y-35);
    realAgentPositionText.setPosition(getCenterPosition(realAgentPositionText, agentWindow).x, getCenterPosition(realAgentPositionText, agentWindow).y+65);
    while (agentWindow.isOpen()) {
        sf::Event agentEvent;
        while (agentWindow.pollEvent(agentEvent)) {
            if (agentEvent.type == sf::Event::Closed || (agentEvent.type == sf::Event::KeyPressed && agentEvent.key.code == sf::Keyboard::Escape)) {
                agentWindow.close();
                stopParam = true;
            }
        }
        agentWindow.clear(sf::Color::White);
        agentWindow.draw(agentPositionText);
        agentWindow.draw(realAgentPositionText);
        agentWindow.draw(header);
        agentWindow.display();
    }

}



int main(int, char const**)
{
    // Create the main window
    window.setFramerateLimit(60);
    
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }

    
    sf::Texture agentTexture;
    if (!agentTexture.loadFromFile(resourcePath() + "plane.png")) {
        return EXIT_FAILURE;
    }
    
    
    double initial_probability = 1.0 / terrainWidth;
    
    std::vector<double> probabilities(terrainWidth, initial_probability);
    
    int agentPosition = getAgentPosition(terrainWidth);
   
    updateProbabilities(agentPosition, probabilities);
    
    
    
    sf::VertexArray graph(sf::LinesStrip, terrainWidth);
    
    
    for (int x = 0; x < terrainWidth; ++x){
        double y = terrain(x);

        graph[x].position = sf::Vector2f(x, y+700);
        if(y == 196){
            graph[x].color = sf::Color::Blue;
        }else if(abs(y-196) <= 30){
            graph[x].color = sf::Color(244, 164, 96);
        }else if(-y > 10){
            graph[x].color = sf::Color(144, 144, 144);
        }else {
            graph[x].color = sf::Color(34, 139, 34);
        }
       
    }
    
    sf::VertexArray probabilityGraph(sf::LinesStrip, terrainWidth);
    
    sf::RectangleShape agentLine(sf::Vector2f(1, window.getSize().y));
    agentLine.setFillColor(sf::Color(241, 74, 74));
    agentLine.setPosition(agentPosition, 420);
    
    sf::Sprite agentSprite;
    agentSprite.setTexture(agentTexture);
    agentSprite.setScale(0.1f, 0.1f);
    
    agentSprite.setPosition(agentPosition-agentSprite.getGlobalBounds().width/2, 400);
    
    
    // Start the game loop
    while (window.isOpen())
    {
        if(!stopParam){
            int currentPos = moveAgent(agentPosition, probabilities);
            moveAction(currentPos, probabilities, agentPosition, agentLine, agentSprite, probabilityGraph);
            int foundPosition = whereIsAgent(accuracy, probabilities);
            
            if(foundPosition > 0){
                //показать окно срезультатами и преостановить другие действия
                callWinWindow(foundPosition, agentPosition);
            }
                
            
            for(auto const &val : probabilities){
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            // stop moment
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                stopParam = !stopParam;
            }
            
            // move agent
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                
                int currentPos = moveAgent(agentPosition, probabilities);
                moveAction(currentPos, probabilities, agentPosition, agentLine, agentSprite, probabilityGraph);
                int foundPosition = whereIsAgent(accuracy, probabilities);
                
                if(foundPosition > 0){
                    //показать окно срезультатами и преостановить другие действия
                    callWinWindow(foundPosition, agentPosition);
                }
                    
                
                for(auto const &val : probabilities){
                    std::cout << val << " ";
                }
                std::cout << std::endl;
                
            }
            
            
            
        }
        
            // Load a sprite to display
        //    sf::Texture texture;
        //    if (!texture.loadFromFile(resourcePath() + "cute_image.jpg")) {
        //        return EXIT_FAILURE;
        //    }
        //    sf::Sprite sprite(texture);
        
        

        // Clear screen
        window.clear();

        // Draw the sprite
        window.draw(graph);
        
        window.draw(probabilityGraph);
        // Draw the string
        window.draw(agentLine);
        window.draw(agentSprite);
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
