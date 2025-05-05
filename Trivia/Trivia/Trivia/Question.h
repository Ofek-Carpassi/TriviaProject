#pragma once
#include <string>
#include <vector>

using std::string;
using wordList = std::vector<string>;

class Question
{	
public:
	Question(string question, wordList possibleAnswers, int correctAnswerId);
	string getQuestion();
	wordList getPossibleAnswers();
	int getCorrectAnswerId();
private:
	string m_question;
	wordList m_possibleAnswers;
	int m_id_correct_answer;
};