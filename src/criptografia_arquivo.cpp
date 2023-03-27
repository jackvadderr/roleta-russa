#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <filesystem>

using namespace std;

class CriptografiaArquivo {
    public:
        /**
         * @brief Função que criptografa um arquivo usando uma chave.
         *
         * @param caminhoArquivo O caminho do arquivo a ser criptografado.
         * @param chave A chave usada para criptografar o arquivo.
         * @throws std::runtime_error Se não for possível abrir o arquivo original para leitura, abrir o arquivo temporário para escrita, remover o arquivo original ou renomear o arquivo temporário.
         */
        static void criptografarArquivo(const std::string& caminhoArquivo, const std::string& chave) {
            std::ifstream arquivoOriginal(caminhoArquivo);
            if (!arquivoOriginal.is_open()) {
                throw std::runtime_error("Não foi possível abrir o arquivo original para leitura");
            }

            std::string caminhoTemp = caminhoArquivo + ".tmp";
            std::ofstream arquivoTemp(caminhoTemp);
            if (!arquivoTemp.is_open()) {
                arquivoOriginal.close();
                throw std::runtime_error("Não foi possível abrir o arquivo temporário para escrita");
            }

            char c;
            size_t pos = 0;
            while (arquivoOriginal.get(c)) {
                c ^= chave[pos++ % chave.size()];
                arquivoTemp.put(c);
            }

            arquivoOriginal.close();
            arquivoTemp.close();

            if (std::remove(caminhoArquivo.c_str()) != 0) {
                throw std::runtime_error("Não foi possível remover o arquivo original");
            }
            if (std::rename(caminhoTemp.c_str(), caminhoArquivo.c_str()) != 0) {
                throw std::runtime_error("Não foi possível renomear o arquivo temporário");
            }
        }

        /**
         * @brief Função que criptografa um arquivo usando uma chave.
         *
         * @param caminhoArquivo O caminho do arquivo a ser criptografado.
         * @param chave A chave usada para criptografar o arquivo.
         * @throws std::runtime_error Se não for possível abrir o arquivo original para leitura, abrir o arquivo temporário para escrita, remover o arquivo original ou renomear o arquivo temporário.
         */
        static void criptografarArquivoAES(const std::string& caminhoArquivo, const std::string& chave) {
            try {
                // Lê o conteúdo do arquivo
                std::ifstream arquivo(caminhoArquivo);
                if (!arquivo.is_open()) {
                    throw std::runtime_error("Não foi possível abrir o arquivo para leitura");
                }
                std::stringstream buffer;
                buffer << arquivo.rdbuf();
                std::string conteudo = buffer.str();

                // Criptografa o conteúdo
                std::string conteudoCriptografado = criptografarConteudoAES(conteudo, chave);
                // Salva o conteúdo criptografado em um arquivo temporário
                std::string caminhoTemp = caminhoArquivo + ".tmp";
                std::ofstream arquivoTemp(caminhoTemp);
                if (!arquivoTemp.is_open()) {
                    throw std::runtime_error("Não foi possível abrir o arquivo temporário para escrita");
                }
                arquivoTemp << conteudoCriptografado;
                arquivoTemp.close();

                // Remove o arquivo original e renomeia o arquivo temporário
                std::filesystem::remove(caminhoArquivo);
                std::filesystem::rename(caminhoTemp, caminhoArquivo);
            }
            catch (const std::exception& e) {
                std::cerr << "Erro ao criptografar o arquivo: " << e.what() << std::endl;
            }
        }

        /**
         * @brief Criptografa o conteúdo de uma string utilizando o algoritmo AES.
         *
         * @param conteudo Conteúdo a ser criptografado.
         * @param chave Chave para a criptografia.
         * @return std::string Conteúdo criptografado.
         * @throw std::runtime_error Se ocorrer um erro na criptografia.
         */
        static std::string criptografarConteudoAES(const std::string& conteudo, const std::string& chave) {
            if (conteudo.empty() || chave.empty() || chave.size() > 32) {
                throw std::invalid_argument("Conteúdo ou chave inválidos.");
            }

            EVP_CIPHER_CTX* contexto = EVP_CIPHER_CTX_new();
            if (contexto == nullptr) {
                throw std::runtime_error("Não foi possível criar o contexto de criptografia.");
            }

            std::string buffer(conteudo.size() + AES_BLOCK_SIZE, 0);
            int tamanho_saida = 0;

            if (EVP_EncryptInit_ex(contexto, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(chave.data()), nullptr) != 1) {
                EVP_CIPHER_CTX_free(contexto);
                throw std::runtime_error("Não foi possível inicializar o algoritmo de criptografia.");
            }

            if (EVP_EncryptUpdate(contexto, reinterpret_cast<unsigned char*>(buffer.data()), &tamanho_saida, reinterpret_cast<const unsigned char*>(conteudo.data()), conteudo.size()) != 1) {
                EVP_CIPHER_CTX_free(contexto);
                throw std::runtime_error("Não foi possível criptografar o conteúdo.");
            }

            int tamanho_final = 0;
            if (EVP_EncryptFinal_ex(contexto, reinterpret_cast<unsigned char*>(buffer.data()) + tamanho_saida, &tamanho_final) != 1) {
                EVP_CIPHER_CTX_free(contexto);
                throw std::runtime_error("Erro ao finalizar a criptografia.");
            }

            EVP_CIPHER_CTX_free(contexto);
            buffer.resize(tamanho_saida + tamanho_final);
            return buffer;
        }

        /**
         * @brief Lê o conteúdo de um arquivo.
         *
         * @param path_file Caminho do arquivo.
         * @return std::string Conteúdo do arquivo.
         * @throw std::runtime_error se não for possível abrir o arquivo.
         */
        static std::string lerConteudoArquivo(const std::string& path_file) {
            cout << "Debug inicio funcao ler conteudo arquivo" << endl;
            std::ifstream arquivo(path_file);
            if (!arquivo.is_open()) {
                throw std::runtime_error("Não foi possível abrir o arquivo: " + path_file);
            }

            // Verifica o tamanho do arquivo e aloca a memória necessária para armazenar o seu conteúdo
            arquivo.seekg(0, std::ios::end);
            std::streamsize size = arquivo.tellg();
            arquivo.seekg(0, std::ios::beg);
            std::string conteudo(size, ' ');

            // Lê o conteúdo do arquivo para a string alocada
            if (!arquivo.read(&conteudo[0], size)) {
                throw std::runtime_error("Não foi possível ler o conteúdo do arquivo: " + path_file);
            }

            arquivo.close();
            cout << "Debug final funcao ler conteudo arquivo" << endl;
            return conteudo;
        }

    private:
        static const int TAMANHO_CHAVE = 256 / 8;
        static const int TAMANHO_BLOCO = 32;
};