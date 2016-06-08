// PL homework: hw1
// fsa.h

#ifndef _PL_HOMEWORK_FSA_H_
#define _PL_HOMEWORK_FSA_H_

#include <map>
#include <set>

#define DFA true
#define NFA_DFA false

struct TableElement {
  int state;
  char input_char;
  int next_state;
};

struct FiniteStateAutomaton {
  // Make your own FSA struct here.
  bool type;
  int initial_state;
  std::set<int> final_states;
  std::map<std::pair<int, char>, int > transitions;
  std::set< std::set<int> > final_states_sets;
  std::map<std::pair<std::set<int>, char>, std::set<int> > transitions_to_set;
};

// Homework 1.1
bool RunFSA(const FiniteStateAutomaton* fsa, const char* str);

// Homework 1.1 and 1.2
bool BuildFSA(const TableElement* elements, int num_elements,
              const int* accept_states, int num_accept_states,
              FiniteStateAutomaton* fsa);

// Homework 1.3
bool BuildFSA(const char* regex, FiniteStateAutomaton* fsa);

std::set<int>& find_lambda_transition(std::set<int>& state_set, std::multimap<std::pair<int, char>, int>& NFA_transitions);

void print_set(const std::set<int>& _set);
#endif //_PL_HOMEWORK_FSA_H_

