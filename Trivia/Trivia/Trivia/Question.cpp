#include "Question.hpp"

Question::Question(string question, wordList possibleAnswers, int correctAnswerId)  :
    m_question(question), m_possibleAnswers(possibleAnswers), m_id_correct_answer(correctAnswerId) {
}

string Question::getQuestion() {
    return this->m_question;
}

wordList Question::getPossibleAnswers() {
    return this->m_possibleAnswers;
}

int Question::getCorrectAnswerId() {
    return this->m_id_correct_answer;
}

