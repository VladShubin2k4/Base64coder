#include <windows.h>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int transfer(int arg, int& k, int radix) {
    int res = 0;
    if (arg < 0) arg += 256;
    k = 1;
    for (int i = 0; arg != 0; ++i) {
        res += (arg % radix) * k;
        k *= 10;
        arg /= radix;
    }
    return res;
}

void Decodetable(string& str, vector<int>& code, bool decode = true) {
    if (decode) {
        for (unsigned i = 0; i < str.length(); ++i) {
            if (isalpha(str[i])) {
                if (str[i] > 95) code[i] = str[i] - 71;
                else code[i] = str[i] - 65;
            }
            else if (isdigit(str[i])) code[i] = str[i] + 4;
            else if (str[i] == '=') code[i] = 0;
            else if (str[i] == '+') code[i] = 62;
            else code[i] = 63;
        }
    }
    else for (unsigned i = 0; i < str.length(); ++i) code[i] = str[i];
}

void SplitIntoDigits(int& p, int n, vector<int>& bin, int& i) {
    while (p) {
        bin[i++] = n / p;
        n %= p;
        p /= 10;
    }
}

void transfer_to_ASCII(int& end, vector<int>& code, vector<int>& bin, int r) {
    for (int i = 0; i < end / r; ++i) {
        code[i] = 0;
        for (int j = i * r; j < i * r + r; ++j) {
            int p = 1 << (r - 1);
            while (p) {
                code[i] = code[i] + (p * bin[j++]);
                p >>= 1;
            }
        }
    }
}

void transfer_to_Base64(int& end, vector<int>& code, vector<int>& bin, int r, string& res) {
    for (int i = 0; i <= end / r; ++i) {
        code[i] = 0;
        for (int j = i * r; j < i * r + r && j < end; ++j) {
            int p = 1 << (r - 1);
            while (p && j < end) {
                code[i] = code[i] + (p * bin[j++]);
                p >>= 1;
            }
            if (code[i] < 26) res += static_cast<char>(code[i] + 65);
            else if (code[i] < 52) res += static_cast<char>(code[i] + 71);
            else if (code[i] < 62) res += static_cast<char>(code[i] - 4);
            else if (code[i] == 62) res += static_cast<char>(43);
            else res += static_cast<char>(47);
        }
    }
}

int transfer_to_BIN(string& str, vector<int>& code, vector<int>& bin, int arg) {
    int p = 1;
    for (int i = 0; i < static_cast<int>(str.length()); ++i) {
        code[i] = transfer(code[i], p, 2);
        p /= 10;
        for (int j = i * arg; j < arg + i * arg; ++j) {
            if (p < pow(10, arg)) {
                if (!p && isalpha(str[i])) {
                    int lim = j;
                    while (j < lim + arg) bin[j++] = 0;
                }
                else {
                    int cntp = p, cnt = 0;
                    while (cntp) {
                        cntp /= 10;
                        cnt++;
                    }
                    for (int r = 0; r < arg - cnt; ++r) bin[j++] = 0;
                    SplitIntoDigits(p, code[i], bin, j);
                }
            }
            else if (p) SplitIntoDigits(p, code[i], bin, j);
        }
    }
    return (static_cast<int>(str.length()) * arg);
}

void format(string& str) {
    for (unsigned i = 0; i < str.length(); ++i)
        if (str[i] == '.') str.insert(str.begin() + i + 1, '\n');
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    ios::sync_with_stdio(false);

    int end;
    string str, success_report = "It's done!\nYou could close this app", output_choose = "Where to output the result: file or console (press f or c)?\nIt's recommended to use a file because console may not have enough buffer size to display all text";

    cout << "encode or decode?\n";
    getline(cin, str);
    cout << "Input your string\n";

    if (str == "de") {
        getline(cin, str);
        vector<int> code(str.length()), bin(str.length() * 6);

        Decodetable(str, code);
        end = transfer_to_BIN(str, code, bin, 6);
        transfer_to_ASCII(end, code, bin, 8);

        cout << output_choose << "\n";

        getline(cin, str);
        if (str == "f") {
            fstream fout("decryption.txt", ios::out);
            str.clear();
            for (int i = 0; i < end >> 3; ++i) str += static_cast<char>(code[i]);
            format(str);
            fout << str;
            fout.close();
        }
        else if (str == "c") {
            str.clear();
            for (int i = 0; i < end >> 3; ++i) str += static_cast<char>(code[i]);
            format(str);
            cout << str << "\n";
        }

        cout << success_report << "\n";
    }
    else if (str == "en") {
        getline(cin, str);
        string res;
        vector<int> code(str.length() << 1), bin(str.length() << 3);

        Decodetable(str, code, false);
        end = transfer_to_BIN(str, code, bin, 8);
        transfer_to_Base64(end, code, bin, 6, res);

        if (end % 6) for (int i = 1; i < 6 - (end % 6); i <<= 1) res += "=";

        cout << output_choose << "\n";
        getline(cin, str);

        if (str == "f") {
            fstream fout("decryption.txt", ios::out);
            fout << res;
            fout.close();
        }
        else if (str == "c") cout << res << "\n";

        cout << success_report << "\n";
    }
    cin.get(); cin.get();
    return 0;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
