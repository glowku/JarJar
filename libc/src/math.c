#include <math.h>
#define LOG10E 0.43429448190325182765
// Simple math functions for kernel

// Absolute value
int abs(int n) {
    return n < 0 ? -n : n;
}

long labs(long n) {
    return n < 0 ? -n : n;
}

// Floating point (simplified)
double fabs(double x) {
    return x < 0 ? -x : x;
}

double floor(double x) {
    if (x >= 0) {
        return (double)(long)x;
    } else {
        return (double)(long)(x - 0.9999999999999999);
    }
}

double ceil(double x) {
    if (x >= 0) {
        return (double)(long)(x + 0.9999999999999999);
    } else {
        return (double)(long)x;
    }
}

double sqrt(double x) {
    if (x < 0) return 0;  // Error case
    if (x == 0) return 0;
    
    double guess = x;
    for (int i = 0; i < 100; i++) {
        double next = 0.5 * (guess + x / guess);
        if (fabs(next - guess) < 0.0000001) {
            return next;
        }
        guess = next;
    }
    return guess;
}

double pow(double base, double exp) {
    if (exp == 0) return 1;
    if (base == 0) return 0;
    if (exp == 1) return base;
    
    // Simple implementation for integer exponents
    if (exp == (long)exp) {
        long n = (long)exp;
        double result = 1;
        double b = base;
        
        if (n < 0) {
            n = -n;
            b = 1 / b;
        }
        
        while (n > 0) {
            if (n & 1) {
                result *= b;
            }
            b *= b;
            n >>= 1;
        }
        return result;
    }
    
    // For non-integer exponents, use approximation
    // This is a simplified version
    return exp2(exp * log2(base));
}

double exp(double x) {
    // Taylor series approximation
    double result = 1;
    double term = 1;
    
    for (int i = 1; i < 50; i++) {
        term *= x / i;
        result += term;
    }
    
    return result;
}

double log(double x) {
    if (x <= 0) return 0;  // Error
    
    // Natural log using Newton's method
    double guess = x;
    for (int i = 0; i < 100; i++) {
        double next = guess - 1 + x / exp(guess);
        if (fabs(next - guess) < 0.0000001) {
            return next;
        }
        guess = next;
    }
    return guess;
}

double log10(double x) {
    return log(x) / LOG10E;
}

double log2(double x) {
    return log(x) / M_LN2;
}

double exp2(double x) {
    return pow(2, x);
}

double sin(double x) {
    // Normalize to [-pi, pi]
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    // Taylor series
    double result = x;
    double term = x;
    
    for (int i = 1; i < 20; i++) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        result += term;
    }
    
    return result;
}

double cos(double x) {
    return sin(x + M_PI / 2);
}

double tan(double x) {
    double c = cos(x);
    if (c == 0) return 0;  // Error
    return sin(x) / c;
}

double atan(double x) {
    // Taylor series for arctan
    double result = 0;
    double term = x;
    double x2 = x * x;
    
    for (int i = 0; i < 100; i++) {
        result += term / (2 * i + 1);
        term *= -x2;
    }
    
    return result;
}

double atan2(double y, double x) {
    if (x > 0) {
        return atan(y / x);
    } else if (x < 0 && y >= 0) {
        return atan(y / x) + M_PI;
    } else if (x < 0 && y < 0) {
        return atan(y / x) - M_PI;
    } else if (x == 0 && y > 0) {
        return M_PI / 2;
    } else if (x == 0 && y < 0) {
        return -M_PI / 2;
    }
    return 0;  // Undefined
}

double fmod(double x, double y) {
    if (y == 0) return 0;  // Error
    return x - trunc(x / y) * y;
}

double trunc(double x) {
    if (x >= 0) {
        return floor(x);
    } else {
        return ceil(x);
    }
}

double round(double x) {
    if (x >= 0) {
        return floor(x + 0.5);
    } else {
        return ceil(x - 0.5);
    }
}

// Floating point classification
int isnan(double x) {
    return x != x;
}

int isinf(double x) {
    return x == INFINITY || x == -INFINITY;
}

int isfinite(double x) {
    return !isnan(x) && !isinf(x);
}

int isnormal(double x) {
    return isfinite(x) && x != 0;
}

int signbit(double x) {
    return x < 0 || (x == 0 && 1.0 / x < 0);
}
