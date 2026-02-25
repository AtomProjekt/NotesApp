#pragma once

#include "Note.h"
#include <vector>
#include <string>

extern const std::string DATA_DIR;
extern const std::string DATA_FILE;

std::string          currentTime();
void                 saveNotes(const std::vector<Note>& notes);
std::vector<Note>    loadNotes();

int         mainMenu   (std::vector<Note>& notes);
int         selectTag  ();
int         pickNote   (const std::vector<Note>& notes, const std::string& action);

std::string noteEditor (const std::string& title, int tagIdx,
                        const std::string& initial = "");

void viewNote    (const Note& n);
void writeNote   (std::vector<Note>& notes);
void editNote    (std::vector<Note>& notes);
void deleteNote  (std::vector<Note>& notes);
void searchNotes (const std::vector<Note>& notes);
