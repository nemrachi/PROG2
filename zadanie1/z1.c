#include <stdio.h>

int main() {
    //------------1------------
    double m; // weight of space probe (kg)
    double T; // engine traction (N)
    double H; // initial height of space probe (m)
    double g; // gravitational pull (m/s^2)
    // max allowed speed of space probe during landing (m/s)
    double v_soft;
    // time step of simulation (in each step - calculate and print state of simulation) 
    double dt;

    scanf("%lf", &m);
    scanf("%lf", &T);
    scanf("%lf", &H);
    scanf("%lf", &g);
    scanf("%lf", &v_soft);
    scanf("%lf", &dt);

    printf("m=%.3lf\n", m);
    printf("T=%.3lf\n", T);
    printf("H=%.3lf\n", H);
    printf("g=%.3lf\n", g);
    printf("vsoft=%.3lf\n", v_soft);
    printf("dt=%.3lf\n", dt);

    //------------2------------
    // space probe acceleration after engine start
    double A = T/m;
    // distance of free fall
    double d_f = ((A - g)*H)/A;
    // critical height
    double d_e = H - d_f;

    printf("de=%.3lf\n", d_e);

    //------------3------------
    short int s = 0; // engine state
    double h = H; // actual high
    double t = 0.0; // time of simulation
    double v = 0.0; // actual speed
    int total = 0; // count rows with s=1
    int max = 0; // longest time when s=1
    int maxmax = 0;
    double a; // current space probe acceleration

    while (h > 0.0) {
        printf("s=%d ", s);
        printf("h=%08.3lf ", h);
        printf("t=%.3lf ", t);
        printf("v=%.3lf\n", v);

        a = (s) ? A - g : -g;
        h = h + v*dt + ((a*dt*dt)/2);
        v = v + a*dt;
        t += dt;

        if (s) total += 1;
        if (s) {
            max += 1;
        } else {
            if (maxmax < max) {
                maxmax = max;
            }
            max = 0;
        }

        // check speed to turn engine on or off
        s = (h <= d_e && v < v_soft) ? 1 : 0;
    }
    if (maxmax < max) {
        maxmax = max;
    }

    //------------4------------
    printf("---Landed---\n");
    printf("%-5c=%08.3lf\n", 'h', h);
    printf("%-5c=%8.3lf\n", 't', t);
    printf("%-5c=%8.3lf\n", 'v', v);
    printf("%-5s=%4d\n", "total", total);
    printf("%-5s=%4d", "max", maxmax);

    return 0;
}