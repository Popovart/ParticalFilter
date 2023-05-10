//
//  usefullFunctions.h
//  sudoku
//
//  Created by Dmitry Popov on 01.05.2023.
//  Copyright © 2023 Dmitry Popov. All rights reserved.
//

#pragma once

template <typename T>
void centerElement(T& element, sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect elementBounds = element.getGlobalBounds();
    element.setOrigin(elementBounds.left + elementBounds.width / 2.0f, elementBounds.top + elementBounds.height / 2.0f);
    element.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
}

template <typename T>
sf::Vector2f getCenterPosition(T& element, sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect elementBounds = element.getGlobalBounds();
    float centerX = windowSize.x / 2.0f - (elementBounds.left + elementBounds.width / 2.0f);
    float centerY = windowSize.y / 2.0f - (elementBounds.top + elementBounds.height / 2.0f);
    return sf::Vector2f(centerX, centerY);
}
