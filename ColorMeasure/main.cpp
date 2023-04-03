#include <stdio.h>
#include <string.h>
#include "scores.h"

long hextonum(char *str)
{
    char *in = str;
    char *pin = in;
    long out = 0;
    while (*pin != 0)
    {
        out <<= 4;
        out += (*pin < 'A') ? *pin & 0xF : (*pin & 0x7) + 9;
        pin++;
    }
    return out;
}

inline float gamma(float x)
{
    return x > 0.04045 ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92;
};

void RGBToLab(int red, int green, int blue, double *lv, double *av, double *bv)
{
    float B = gamma(blue / 255.0f);
    float G = gamma(green / 255.0f);
    float R = gamma(red / 255.0f);
    float X = 0.412453 * R + 0.357580 * G + 0.180423 * B;
    float Y = 0.212671 * R + 0.715160 * G + 0.072169 * B;
    float Z = 0.019334 * R + 0.119193 * G + 0.950227 * B;

    X /= 0.95047;
    Y /= 1.0;
    Z /= 1.08883;

    float FX = X > 0.008856f ? pow(X, 1.0f / 3.0f) : (7.787f * X + 0.137931f);
    float FY = Y > 0.008856f ? pow(Y, 1.0f / 3.0f) : (7.787f * Y + 0.137931f);
    float FZ = Z > 0.008856f ? pow(Z, 1.0f / 3.0f) : (7.787f * Z + 0.137931f);
    *lv = Y > 0.008856f ? (116.0f * FY - 16.0f) : (903.3f * Y);
    *av = 500.f * (FX - FY);
    *bv = 200.f * (FY - FZ);
}

int main()
{
    char colors[] = "#23171b    #4860e6    #2aabee    #2ee5ae    #6afd6a    #c0ee3d    #feb927    #fe6e1a    #c2270a    #900c00";

    char split[] = "    ";
    char *col = strtok(colors, split);

    char *color_codes[10];
    color_codes[0] = col;

    col = strtok(NULL, split);

    int idcol = 1;
    while (col)
    {
        color_codes[idcol] = col;

        col = strtok(NULL, split);
        idcol += 1;
    }

    int reds[10];
    int greens[10];
    int blues[10];
    for (int i = 0; i < 10; i++)
    {
        long num = hextonum(color_codes[i]);
        int red = num >> 16 & 0xFF;
        int green = num >> 8 & 0xFF;
        int blue = num & 0xFF;

        reds[i] = red;
        greens[i] = green;
        blues[i] = blue;
    }

    double pd = 0, nd = 0, pp = 0, nu = 0;
    for (int i = 0; i < 10; i++)
    {
        double l1, a1, b1, l2, a2, b2;

        RGBToLab(reds[i], greens[i], blues[i], &l1, &a1, &b1);
        if (i != 9) {
            RGBToLab(reds[i + 1], greens[i + 1], blues[i + 1], &l2, &a2, &b2);
        } else {
            RGBToLab(reds[0], greens[0], blues[0], &l2, &a2, &b2);
        }
        
        pd += ciede2000(l1, a1, b1, l2, a2, b2);
        nd += nameDifference(l1, a1, b1, l2, a2, b2);
        pp += pairPreference(l1, a1, b1, l2, a2, b2);
        nu += nameUniqueness(l1, a1, b1);
    }
    
    printf("%f\t%f\t%f\t%f\n", pd, nd, pp, nu);
    
    return 0;
}
