
// Параметры и графика
#include "Param.h"

// Позиции игрока и NPC
const Vector2u STARTPAC(float(((WIDTH_MAP - 1.15)* FACECOEFF_W) / 2), float(HEIGHT_MAP* FACECOEFF_H * 0.722));
const Vector2u GHOST_BL(float(((WIDTH_MAP - 1.15)* FACECOEFF_W) / 2), float(HEIGHT_MAP* FACECOEFF_H * 0.45));

const Vector2u OTHER_GH(float(((WIDTH_MAP + 6.8)* FACECOEFF_W) / 2), HEIGHT_MAP* FACECOEFF_H * 0.49);
// Установка начальной позиции под призраков
// _________________________________________

void setGhostPosition(Ghost* ghosts) {

    ghosts[0].startInit(GHOST_BL.x, GHOST_BL.y, UP);
    for (int i = 1; i < 4; i++)
    {
        ghosts[i].startInit(OTHER_GH.x - 2 * i * FACECOEFF_W, OTHER_GH.y, UP);
    }
}

void setGhostPosition(Ghost ghosts, int index) {

    ghosts.startInit(OTHER_GH.x - 2 * index * FACECOEFF_W, OTHER_GH.y, UP);
}

int main()
{
    srand(unsigned(time(NULL)));

    float time_counter = 0.0;
    int ind_ghostOut = 1;
    
    // Объявление и загрузка(инициализация) текстур
    Texture textureMap;
    Texture textureObject;

    textureMap.loadFromFile("TextureManager/playField.png");
    textureObject.loadFromFile("TextureManager/objectGame.png");

    // Текстуры в объекте
    Sprite spriteMap(textureMap);
    Sprite spriteObject(textureObject);
    Sprite spriteNPC(textureObject);

    // Масштабирование под форму
    spriteMap.scale(Vector2f((float)(WIDTH_MAP * FACECOEFF_W) / (textureMap.getSize()).x, (float)(HEIGHT_MAP * FACECOEFF_H) / (textureMap.getSize()).y));
    spriteObject.scale(Vector2f((float)((FACECOEFF_H + FACECOEFF_W) * 0.5) / (textureObject.getSize().x / 12), (float)((FACECOEFF_H + FACECOEFF_W) * 0.5) / (textureObject.getSize().y / 8)));
    
    // Настройка формы
    RenderWindow window(VideoMode(float(WIDTH_MAP * FACECOEFF_W), float(HEIGHT_MAP * FACECOEFF_H)), "(_Pac-Man_)");

    // Начальные настройки ПакМан-а
    // Start-position
    // Создание и инициализация
    PacMan pac;
    pac.startInit(STARTPAC.x, STARTPAC.y, RIGHT);
    ///
    spriteNPC.setPosition(pac.curr_X, pac.curr_Y);
    // Масштабирование NPC
    spriteNPC.scale(Vector2f((float)((FACECOEFF_H + FACECOEFF_W) * 0.5) / (textureObject.getSize().x / 13.5), (float)((FACECOEFF_H + FACECOEFF_W) * 0.5) / (textureObject.getSize().y / 9)));
    ///

    // Настройки призравков
    // Statrt place
    // Array ghosts : всего 4 призрака
    Ghost* ghosts = new Ghost[4];
    // Запуск первого красного
    setGhostPosition(ghosts);
    ghosts[0].ghostOut();

    //, where ---> 1-Blinky; 2-Clyde; 3-Inky; 4-Pinky
    // ______________________________________________

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                pac.buf_X = pac.curr_X;
                pac.buf_Y = pac.curr_Y;

                if (event.key.code == Keyboard::Right) {
                    pac.rotSide = RIGHT;
                }
                else if (event.key.code == Keyboard::Left) {
                    pac.rotSide = LEFT;
                }
                else if (event.key.code == Keyboard::Up) {
                    pac.rotSide = UP;
                }
                else if (event.key.code == Keyboard::Down) {
                    pac.rotSide = DOWN;
                }

            }
        }

        window.clear();
        window.draw(spriteMap);

        // Рисовка ПакМан-а
        pac.update();
        spriteNPC.setTextureRect(IntRect((round(pac.frame) + 2 * pac.rotSide) * (CATCH_SPLIT_W - 1), 2*(CATCH_SPLIT_H + CUT_CORRECT), CATCH_SPLIT_W - CUT_CORRECT, CATCH_SPLIT_H));
        spriteNPC.setPosition(pac.curr_X, pac.curr_Y);
        window.draw(spriteNPC);
        // __________________________________________________________________________________

        // Рисовка призраков
        for (int i = 0; i < 4; i++)
        {
            ghosts[i].update();
            spriteNPC.setTextureRect(IntRect(ghosts[i].rotSide * (CATCH_SPLIT_W), (3 + i) * (CATCH_SPLIT_H), CATCH_SPLIT_W, CATCH_SPLIT_H));
            spriteNPC.setPosition(ghosts[i].curr_X, ghosts[i].curr_Y);
            window.draw(spriteNPC);
        } 

        // Высвобождение призраков
        if (ind_ghostOut <= 3) {

            time_counter += 0.001;
        }
        if (time_counter > 20) {
            ghosts[ind_ghostOut++].ghostOut();
            time_counter = 0.0;
        }

        //spriteNPC.setTextureRect(IntRect((2 * 0) * (CATCH_SPLIT_W - 1), 3 * (CATCH_SPLIT_H), CATCH_SPLIT_W, CATCH_SPLIT_H));
        //spriteNPC.setPosition(ghosts[0].curr_X, ghosts[0].curr_Y);
        //window.draw(spriteNPC);
        // __________________________________________________________________________________

        for (int i = 0; i < HEIGHT_MAP; i++)
        {
            for (int j = 0; j < WIDTH_MAP; j++)
            {

                if (gameField[i][j] == '.') { // Pill

                    spriteObject.setTextureRect(IntRect(CUT_CORRECT, CUT_CORRECT, CATCH_SPLIT_W, CATCH_SPLIT_H));
                }
                else if (gameField[i][j] == '*') { // BigPill

                    spriteObject.setTextureRect(IntRect(CATCH_SPLIT_W + CUT_CORRECT, CUT_CORRECT, CATCH_SPLIT_W, CATCH_SPLIT_H));
                }
                else {
                    continue;
                }

                // Установление позиции и рисовка
                spriteObject.setPosition(float(j * FACECOEFF_W), float(i * FACECOEFF_H));
                window.draw(spriteObject);
            }
        }

        window.display();
    }

    return 0;
    
}