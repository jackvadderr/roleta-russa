#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <filesystem>


std::vector<std::string> list_files(const std::filesystem::path& dir_path);
int sorteio();
void salva_arquivo();
int le_arquivo();

#endif // FUNCTIONS_H


