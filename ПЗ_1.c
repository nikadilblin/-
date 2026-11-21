#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Прототипы функций
double Y(double x);
double V(double x);
void print_table(double (*func)(double), double xn, double xk, double h, const char* name);
void plot_graph(double (*func)(double), double x0, double x1, const char* name);

int main() {
    int choice;
    double xn, xk, h;

    while (1) {
        printf("\n--- МЕНЮ (Подзадача 1) ---\n");
        printf("1. Табулировать Y(x)\n");
        printf("2. Табулировать V(x)\n");
        printf("3. Построить график Y(x)\n");
        printf("4. Построить график V(x)\n");
        printf("5. Выход\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);

        if (choice == 5) break;

        if (choice >= 1 && choice <= 4) {
            printf("Введите начало интервала (xn): ");
            scanf("%lf", &xn);
            printf("Введите конец интервала (xk): ");
            scanf("%lf", &xk);
            printf("Введите шаг (h): ");
            scanf("%lf", &h);

            if (h <= 0) {
                printf("Ошибка: шаг должен быть больше нуля!\n");
                continue;
            }

            switch (choice) {
            case 1: print_table(Y, xn, xk, h, "Y(x)"); break;
            case 2: print_table(V, xn, xk, h, "V(x)"); break;
            case 3: plot_graph(Y, xn, xk, "Y(x)"); break;
            case 4: plot_graph(V, xn, xk, "V(x)"); break;
            }
        }
        else {
            printf("Неверный пункт меню.\n");
        }
    }
    return 0;
}

// =========================================
// РЕАЛИЗАЦИЯ МАТЕМАТИЧЕСКИХ ФУНКЦИЙ
// ==========================================

// Функция Y(x)
double Y(double x) {
    // Проверка ОДЗ: деление на ноль при x = 0
    if (fabs(x) < 1e-9) {
        return NAN; // Not a Number (недопустимое значение)
    }

    double numerator = sin(x * x + pow(x, 3) + pow(x, 5)) * pow(10, -x);
    // log10 - это десятичный логарифм (lg)
    double denominator = sqrt(log10(exp(x * x) + x));

    // Дополнительная защита (хотя аналитически знаменатель > 0 при x != 0)
    if (isnan(denominator) || denominator == 0) return NAN;

    return numerator / denominator;
}

// Кусочная функция V(x)
double V(double x) {
    if (x > 2.5) {
        // Ветка 1: x > 2.5
        return sqrt(2 * pow(x, 3) - 1);
    }
    else if (x >= 0 && x <= 2.5) {
        // Ветка 2: 0 <= x <= 2.5
        if (fabs(x - 1.0) < 1e-9) {
            return NAN; // При x=1 ln|1-x| = ln(0) - ошибка
        }
        double inner_log = log(fabs(1 - x)); // натуральный логарифм ln
        double outer_arg = fabs(3 - inner_log);

        if (outer_arg < 1e-9) return NAN; // Защита от ln(0)

        return log(outer_arg);
    }
    else {
        // Ветка 3: x < 0
        return exp(-cos(x * x));
    }
}

// ==========================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (Таблица и График)
// ==========================================

// Функция для печати таблицы
void print_table(double (*func)(double), double xn, double xk, double h, const char* name) {
    printf("\nТабуляция %s:\n", name);
    printf("----------------------\n");
    printf("|   x   |   %-6s   |\n", name);
    printf("----------------------\n");

    for (double x = xn; x <= xk + 1e-9; x += h) {
        double y = func(x);
        if (isnan(y)) {
            printf("| %5.2f |  ERROR  |\n", x);
        }
        else {
            printf("| %5.2f | %7.4f |\n", x, y);
        }
    }
    printf("----------------------\n");
}

// Функция для построения ASCII-графика в консоли
void plot_graph(double (*func)(double), double x0, double x1, const char* name) {
    int width = 60, height = 20;
    double y_min = 1e9, y_max = -1e9;
    double step = (x1 - x0) / (width - 1);

    // Находим минимум и максимум для масштабирования
    for (int i = 0; i < width; i++) {
        double x = x0 + i * step;
        double y = func(x);
        if (isnan(y)) continue;
        if (y < y_min) y_min = y;
        if (y > y_max) y_max = y;
    }

    if (y_max == y_min) {
        printf("Функция постоянна на этом интервале или везде ошибки (NAN), график не построить.\n");
        return;
    }

    // Создаем и инициализируем поле
    char screen[20][60];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) screen[i][j] = ' ';

    // Отрисовка оси X (y=0)
    int zero_y = (int)((y_max - 0) / (y_max - y_min) * (height - 1));
    if (zero_y >= 0 && zero_y < height) {
        for (int j = 0; j < width; j++) screen[zero_y][j] = '-';
    }
    // Отрисовка оси Y (x=0, если попадает в диапазон)
    int zero_x = (int)((0 - x0) / (x1 - x0) * (width - 1));
    if (zero_x >= 0 && zero_x < width) {
        for (int i = 0; i < height; i++) screen[i][zero_x] = '|';
    }

    // Наносим точки функции
    for (int j = 0; j < width; j++) {
        double x = x0 + j * step;
        double y = func(x);
        if (isnan(y)) continue;

        int i = (int)((y_max - y) / (y_max - y_min) * (height - 1));
        if (i >= 0 && i < height) {
            screen[i][j] = '*';
        }
    }

    // Вывод на экран
    printf("\nГрафик %s на интервале [%g, %g]:\n", name, x0, x1);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) putchar(screen[i][j]);
        putchar('\n');
    }
    printf("\n");
}