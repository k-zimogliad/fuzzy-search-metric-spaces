#include "Distance.hpp"
#include <vector>
#include <algorithm>

size_t levenshtein_distance(std::string_view s1, std::string_view s2) {
    // s1 должна быть короче s2 для минимального расхода памяти
    if (s1.length() > s2.length()) {
        return levenshtein_distance(s2, s1);
    }

    const size_t m = s1.length();
    const size_t n = s2.length();

    // Текущая и предыдущая строки матрицы расстояний
    std::vector<size_t> previous_row(m + 1);
    std::vector<size_t> current_row(m + 1);

    // Заполнение базовых значений
    for (size_t i = 0; i <= m; ++i) {
        previous_row[i] = i;
    }

    for (size_t j = 1; j <= n; ++j) {
        current_row[0] = j;
        for (size_t i = 1; i <= m; ++i) {
            // Стоимость замены: 0 если символы равны, иначе 1
            size_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

            current_row[i] = std::min({
                current_row[i - 1] + 1,         // Вставка
                previous_row[i] + 1,            // Удаление
                previous_row[i - 1] + cost      // Замена
                });
        }
        // Меняем строки местами для следующего шага
        std::swap(previous_row, current_row);
    }

    return previous_row[m];
}