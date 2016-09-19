/*********************************************************
** color.h
** Primary author: Matthew B
** Description: color class listing a bunch of common index colors (see end)
** Used in sim:
*********************************************************/

//todo: work on interface for color class. its kinda of lame right now

#ifndef _COLOR_H
#define _COLOR_H

class CColor
{
public:

    inline float r() const {
        return red;
    }
    inline float g() const {
        return green;
    }
    inline float b() const {
        return blue;
    }
    inline float a() const {
        return alpha;
    }


    CColor (float rr, float rg, float rb, float ra) :
            red(rr), green(rg), blue(rb), alpha(ra) {}


    CColor () :
            red(0.0), green(0.0), blue(0.0), alpha(0.0) {}

    void sa(float aaa)
    {
        alpha = aaa;
    }

    inline bool operator==(CColor & B)
    {
        return (red == B.r() && blue == B.b() && green == B.g());
    }
    inline bool operator!=(CColor & B)
    {
        return !(*this == B);
    }

private:
    float red;
    float green;
    float blue;
    float alpha;
};

//these colors should be right
const CColor RED               (1.0, 0.0, 0.0, 1.0);
const CColor GREEN             (0.0, 1.0, 0.0, 1.0);
const CColor BLUE              (0.0, 0.0, 1.0, 1.0);
const CColor GREY              (0.9, 0.9, 0.9, 1.0);

//half tones
const CColor MRED               (0.7, 0.0, 0.0, 1.0);
const CColor MGREEN             (0.0, 0.7, 0.0, 1.0);
const CColor MBLUE              (0.0, 0.0, 0.7, 1.0);
const CColor MGREY               (0.7, 0.7, 0.7, 1.0);

//quarter tones
const CColor DRED               (0.35, 0.0, 0.0, 1.0);
const CColor DGREEN             (0.0, 0.35, 0.0, 1.0);
const CColor DBLUE              (0.0, 0.0, 0.35, 1.0);
const CColor DGREY              (0.3, 0.3, 0.3, 1.0);

//basic mix
const CColor YELLOW            (1.0, 1.0, 0.0, 1.0);
const CColor CYAN              (0.0, 1.0, 1.0, 1.0);
const CColor MAGENTA           (1.0, 0.0, 1.0, 1.0);

const CColor ORANGE            (1.0, 0.5, 0.0, 1.0);
const CColor VIOLET            (148.0/255.0, 0.0, 211.0/255.0, 1.0);

//monotones
const CColor BLACK             (0.0, 0.0, 0.0, 1.0);
const CColor WHITE             (1.0, 1.0, 1.0, 1.0);


//SKIN TONES
const CColor BEIGE             (111/255.0, 255/255.0, 255/255.0, 1.0);
const CColor BROWN             (138/255.0, 69/255.0, 19/255.0, 1.0);
const CColor TAN               (210/255.0,180/255.0,140/255.0, 1.0);
const CColor PEACH             (255/255.0,229/255.0,180/255.0, 1.0);
const CColor APRICOT           (251/255.0,226/255.0,157/255.0, 1.0);
const CColor CORAL             (255/255.0,127/255.0, 80/255.0, 1.0);
const CColor CHOCOLATE         (210/255.0,105/255.0, 30/255.0, 1.0);
const CColor SALMON            (255/255.0,140/255.0,105/255.0, 1.0);
const CColor WHEAT             (245/255.0,222/255.0,179/255.0, 1.0);
const CColor CREAM             (255/255.0,253/255.0,108/255.0, 1.0);
const CColor PINK              (255/255.0,192/255.0,203/255.0, 1.0);
const CColor KHAKI             (240/255.0,230/255.0,140/255.0, 1.0);
const CColor IVORY             (255/255.0,255/255.0,140/255.0, 1.0);


//other very common color names
const CColor PURPLE            (147.0/255.0, 112.0/255.0, 219.0/255.0, 1.0);
const CColor AMBER             (255/255.0,192/255.0,  0/255.0, 1.0);
const CColor HOTPINK           (255/255.0,105/255.0,180/255.0, 1.0);
const CColor LEMON             (253/255.0,233/255.0, 16/255.0, 1.0);
const CColor LIME              (191/255.0,255/255.0,  0/255.0, 1.0);
const CColor MINT              (152/255.0,255/255.0,152/255.0, 1.0);
const CColor NAVY              (  0/255.0,  0/255.0,128/255.0, 1.0);
const CColor SAFETY            (255/255.0,102/255.0,  0/255.0, 1.0);

const int NUM_ALLCOLORS = 39;
const CColor ALLCOLORS[NUM_ALLCOLORS] = {RED, GREEN, BLUE, GREY,
                                        MRED, MGREEN, MBLUE, MGREY, DRED, DBLUE, DGREEN, DGREY,
                                        YELLOW, CYAN, MAGENTA, ORANGE, VIOLET, BLACK, WHITE,
                                        BROWN, TAN, PEACH, APRICOT, CORAL, CHOCOLATE, SALMON, WHEAT, CREAM, PINK, KHAKI, IVORY,
                                        PURPLE, AMBER, HOTPINK, LEMON, LIME, MINT, NAVY, SAFETY
                                        };

const int NUM_BRIGHTS = 8;
const CColor BRIGHTS[NUM_BRIGHTS] = { MAGENTA, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, VIOLET};

const int NUM_SKINTONES = 12;
const CColor SKINTONES[NUM_SKINTONES] = { BROWN, TAN, PEACH, APRICOT, CORAL, CHOCOLATE, SALMON, WHEAT, CREAM, PINK, KHAKI, IVORY};

#endif

