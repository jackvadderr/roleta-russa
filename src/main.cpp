#include <iostream>
#include <ctime>
#include <cstdlib> // Acesso a funcoes do C
#include <fstream> // Para manipulacao de arquivos
#include <string>
#include <vector>
#include <openssl/evp.h> // Funcoes para criptografia
#include <filesystem>

#include "functions.h"

#define INPUT_FILE "aa.txt"
#define SENHA "1234"

using namespace std;


struct OSInfo {
    std::string os_name;
    std::string dir_path;

    OSInfo() {
        #ifdef _WIN32
            os_name = "Windows";
            dir_path = "C:\\";
        #elif __APPLE__
            os_name = "MacOS";
            dir_path = "/Users/";
        #elif __linux__
            os_name = "Linux";
            dir_path = "/home";
        #else
            os_name = "Outro";
            dir_path = "/";
        #endif
    }
};

vector<string> list_files(const std::filesystem::path& dir_path) {
    vector<string> files; // Vetor para armazenar os caminhos completos dos arquivos

    // Percorre todos os arquivos e subdiretórios do diretório atual
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            // Se o item encontrado for um arquivo, adiciona o caminho completo ao vetor de arquivos
            files.push_back(entry.path().string());
        }
    }

    return files; // Retorna o vetor de arquivos encontrados
}

int main(){
    OSInfo os_info;

    int numero_sorteado = sorteio();
    printf("%d\n", numero_sorteado);
    if(numero_sorteado == 6){
        printf("Parabens voce foi o sortudo!");
    }

    // Altera aqui o diretorio
    std::vector<std::string> files = list_files("/"); // Chama a função passando o diretório raiz como argumento

    // Imprime o caminho completo dos arquivos encontrados
    for (const auto& file : files) {
        std::cout << file << std::endl;
    }

    return 0;
}

int sorteio(){
    srand(time(NULL)); // Gera o seed do numero aleatorio
    return rand() % 6 + 1; // returna um numero entre 0 e 6
}

vector<string> le_arquivo(string name_file){
    ifstream arquivo;
    if (!arquivo) {
        cerr << "Error: Nao foi possivel abrir o arquivo." << INPUT_FILE << endl;
    }
    arquivo.open(name_file);
    vector<string> file_content;

    return file_content;
}

void salva_arquivo();