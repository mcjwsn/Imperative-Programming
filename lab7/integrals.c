#include <stdio.h>
#include <math.h>

#define RECURS_LEVEL_MAX  10
#define N_MAX 10

// pointer to function of one variable
typedef double (*Func1vFp)(double);

// example functions of one variable
double f_poly(double x) { return 2*pow(x,5)-4*pow(x,4)+3.5*pow(x,2)+1.35*x-6.25;
}

double f_rat(double x) {return 1.0/(0.01 + (x-0.5)*(x-0.5));
}

double f_exp(double x) {return  2.0*x*exp(-1.5*x)-1;
}

double f_trig(double x) {return x*tan(x);
}

// Quadratures

// rectangle rule, leftpoint
double quad_rect_left(Func1vFp f1, double a, double b, int n) {
	double dx = (b-a)/(double)n;
	double res = 0;
	for (int i =0;i<n;i++){res += dx*f1(a + i*dx);}
	return res;
}

// rectangle rule, rightpoint
double quad_rect_right(Func1vFp f1, double a, double b, int n) {
	double dx = (b-a)/(double)n;
	double res = 0;
	for (int i =0;i<n;i++){res += dx*f1(a + (i+1)*dx);}
	return res;
}

// rectangle rule, midpoint
double quad_rect_mid(Func1vFp f1, double a, double b, int n) {double dx = (b-a)/(double)n;
	double res = 0;
	for (int i =0;i<n;i++){res += dx*f1(a + i*dx + dx*0.5); }
	return res;
}
// trapezoidal rule
double quad_trap(Func1vFp func, double a, double b, int n) {double dx = (b - a)/(double)n;
    double res = 0;

    for(int i = 0; i < n; i++){
        res += 0.5*dx*(func(a + i*dx) + func(a + (i + 1)*dx));
    }
    return res;
}

// Simpson's rule
double quad_simpson(Func1vFp f, double a, double b, int n) {double dx = (b - a)/(double)n;

    double sum_i = 0;
    double sum_t = 0;

    for(int i = 1; i <= n; i++){
        sum_t += f(a + i*dx - dx/2.0f);
        if(i < n)
            sum_i += f(a + i*dx);
    }

    return dx/6.0f*(f(a) + f(b) + 2*sum_i + 4*sum_t);
}

// pointer to quadrature function
typedef double (*QuadratureFp)(Func1vFp, double, double, int);

// array of pointers to integrand functions
Func1vFp func_tab[] = { f_poly, f_rat, f_trig, f_exp };

// array of pointers to quadrature functions
QuadratureFp quad_tab[] = {
	quad_rect_left, quad_rect_right, quad_rect_mid, quad_trap, quad_simpson };

// calls 'quad_no' quadrature function for 'fun_no' integrand function
// on interval [a, b] and n subintervals
double quad_select(int fun_no, int quad_no, double a, double b, int n) {
return quad_tab[quad_no](func_tab[fun_no], a, b, n);}

// adaptive algorithm
double recurs(Func1vFp f, double a, double b, double S, double delta, QuadratureFp quad, int level) {
double c = (a+b)/2.0f;
    double first_half = quad(f, a, c, 1);
    double second_half = quad(f, c, b, 1);
	if(fabs((first_half + second_half) - S) <= delta)
        return first_half + second_half;
    else {
        if(level == RECURS_LEVEL_MAX)
            return NAN;

        return recurs(f, a, c, first_half, delta/2.0f, quad, level + 1)
                    + recurs(f, c, b, second_half, delta/2.0f, quad, level + 1);}}

// initialization for adaptive algorithm
double init_recurs(Func1vFp f, double a, double b, double delta, QuadratureFp quad) {return recurs(f, a, b, quad(f, a, b, 1), delta, quad, 0);}

// double integrals

// pointer to function of two variables
typedef double (*Func2vFp)(double, double) ;

double func2v_2(double x, double y) {return 2 - x*x - y*y*y;
}

// sample functions to define the normal domain

double lower_bound2(double x) {
	return 0.7*exp(-2*x*x);
}
double upper_bound2(double x) {
	return sin(10*x);
}

// rectangle rule (leftpoint) - double integral over rectangular domain
double dbl_integr(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny) {
double dx = (x2 - x1)/(double)nx;
    double dy = (y2 - y1)/(double)ny;
    double integral = 0;

    for(int y = 0; y < ny; y++){
        for(int x = 0; x < nx; x++){
            integral += f(x1 + x*dx, y1 + y*dy);
        }
    }

    return dx*dy*integral;}

// rectangle rule (midpoint) - double integral over normal domain with respect to the x-axis
double dbl_integr_normal_1(Func2vFp f, double x1, double x2, int nx, double hy,
						   Func1vFp fg, Func1vFp fh) {double dx = (x2 - x1)/nx;

    double integral = 0;

    for(int x = 0; x < nx; x++){
        double xi = x1 + (x+0.5)*dx;

        int ny = (int)ceil((fh(xi) - fg(xi))/hy);
        double dy = (fh(xi) - fg(xi))/ny;

        for(int y = 0; y < ny; y++){
            integral += dy*dx*f(xi, fg(xi) + y*dy + 0.5*dy);
        }
    }

    return integral;
}
double min(double a,double b){
    return a<b?a:b;
}

double max(double a,double b){
    return a>b?a:b;
}
// rectangle rule (leftpoint) - double integral over multiple normal
// domains with respect to the x-axis
double quad_rect_left2(Func2vFp f2, double a, double b, double constant, int index, int n) {
    double h=(b-a)/n,suma=0.0;
    for (int i=0; i<n; i++){
        if (index==1) suma+=h*f2(a+i*h,constant);
        else suma+=h*f2(constant,a+i*h);
    }
    return suma;
}
double dbl_integr_normal_n(Func2vFp f, double x1, double x2, int nx, double y1, double y2,
		int ny, Func1vFp fg, Func1vFp fh)  {
            double a,b,hy=(y2-y1)/ny,hx=(x2-x1)/nx,suma=0.0;
    int ny2;
    for (int i=0; i<nx; i++){
        a=max(fg(x1+i*hx),y1);
        b=min(fh(x1+i*hx),y2);
        if(b>a){
            ny2=ceil((b-a)/hy);
            suma+=hx*quad_rect_left2(f,a,b,x1+i*hx,2,ny2);
        }
    }
    return suma;
}

// multiple quadratures

typedef double (*FuncNvFp)(const double*, int);
typedef int (*BoundNvFp)(const double*, int);

// sample function of three variables
double func3v(const double v[], int n) {
	return v[0] - v[1] + 2*v[2];
}

// sample predicate in 3D
int bound3v(const double v[], int n) { // a cylinder
	return v[0] > 0 && v[0] < 0.5 && v[1]*v[1] + (v[2]-1)*(v[2]-1) < 1;
}

// multiple integrals over a cuboid with predicate (if boundary != NULL)
// rectangular rule (rightpoint)
double trpl_quad_rect(FuncNvFp f, double variable_lim[][2], const int tn[], BoundNvFp boundary) {
double dx = (variable_lim[0][1] - variable_lim[0][0])/tn[0];
    double dy = (variable_lim[1][1] - variable_lim[1][0])/tn[1];
    double dz = (variable_lim[2][1] - variable_lim[2][0])/tn[2];

    double integral = 0;

    double xi = variable_lim[0][0];
    for(int x = 0; x < tn[0]; x++){
        xi += dx;
        double yi = variable_lim[1][0];

        for(int y = 0; y < tn[1]; y++){
            yi += dy;
            double zi = variable_lim[2][0];
            for(int z = 0; z < tn[2]; z++){
                zi += dz;

                double v[3] =  {xi, yi, zi};

                if((boundary != NULL && boundary(v, 3) != 0 ) || boundary == NULL)
                    integral += dx*dy*dz*f(v, 3);
            }
        }
    }

    return integral;
}

int main(void) {
int to_do, n, nx, ny, integrand_fun_no, method_no, n_tests;
	int no_funcs = sizeof(func_tab) / sizeof(Func1vFp);
	int no_quads = sizeof(quad_tab) / sizeof(QuadratureFp);
	double a, b, x1, x2, y1, y2, hy, delta;
//	double t_variable[N_MAX], variable_lim[N_MAX][2];
	double variable_lim[N_MAX][2];
	int tn[N_MAX];

	scanf("%d", &to_do);
	switch (to_do) {
		case 1: // loop over quadratures and integrands
			scanf("%lf %lf %d", &a, &b, &n);
			for(int q = 0; q < no_quads; ++q) {
				for(int f = 0; f < no_funcs; ++f) {
					printf("%.5f ",quad_select(f, q, a, b, n));
				}
				printf("\n");
			}
			break;
		case 2: // adaptive algorithm
			scanf("%d",&n_tests);
			for (int i = 0; i < n_tests; ++i) {
				scanf("%d %d", &integrand_fun_no, &method_no);
				scanf("%lf %lf %lf", &a, &b, &delta);
				printf("%.5f\n", init_recurs(func_tab[integrand_fun_no], a, b, delta, quad_tab[method_no]));
			}
			break;
		case 3: // double integral over a rectangle
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf %lf %d", &y1, &y2, &ny);
			printf("%.5f\n", dbl_integr(func2v_2, x1, x2, nx, y1, y2, ny));
			break;
		case 4: // double integral over normal domain
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf", &hy);
			printf("%.5f\n", dbl_integr_normal_1(func2v_2, x1, x2, nx, hy, lower_bound2, upper_bound2));
			break;
		case 5: // double integral over multiple normal domains
			scanf("%lf %lf %d", &x1, &x2, &nx);
			scanf("%lf %lf %d", &y1, &y2, &ny);
			printf("%.5f\n",dbl_integr_normal_n(func2v_2, x1, x2, nx, y1, y2, ny, lower_bound2, upper_bound2));
			break;
		case 6: // triple integral over a cuboid
			scanf("%lf %lf %d", &variable_lim[0][0], &variable_lim[0][1], tn);
			scanf("%lf %lf %d", &variable_lim[1][0], &variable_lim[1][1], tn+1);
			scanf("%lf %lf %d", &variable_lim[2][0], &variable_lim[2][1], tn+2);
//			scanf("%d", &flag);
			printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, NULL));
			printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, bound3v));
			break;
		default:
			printf("Nothing to do for %d\n", to_do);
			break;
	}
	return 0;
}