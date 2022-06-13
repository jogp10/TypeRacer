/** @defgroup mouse_collision mouse_collision
 * @{
 *
 * mouse collisions detection
 */

#define true 1
#define false 0

typedef struct Point
{
    int x;
    int y;
}Point;

/**
 * @brief 
 * 
 * @param opt  area to search
 * @param mouse location of mouse
 * @return true on mouse inside opt
 */
int isInsideOpt(Point opt[], Point mouse);

/**@}*/
