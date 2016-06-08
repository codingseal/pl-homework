// PL homework: hw1
// fsa.cc

#include <iostream>
#include <string.h>
#include <queue>
#include "fsa.h"
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

const char kEps = '#';

char alphabet[2] = {'a', 'b'};
using namespace std;

bool CheckIfNFA(const TableElement* elements, int num_elements) {
  set<pair<int, char> > hash;
  pair<set<pair<int, char> >::iterator, bool> ret;
  hash.clear();
  for (int i = 0; i < num_elements; i++) {
	if (elements[i].input_char == '#') return true;
    ret = hash.insert(make_pair(elements[i].state, elements[i].input_char));
    if (ret.second == false) return true;
  }
  return false;
}

bool BuildDFA(const TableElement* elements, int num_elements,
              const int* accept_states, int num_accept_states,
              FiniteStateAutomaton* fsa) {
  // Check the number of elements
  cout << "num_elements: " << num_elements << endl;
  if (num_elements <= 0) return false;
  
  fsa->type = DFA;
  // Set the initial state
  fsa->initial_state = elements[0].state;

  // Set the final states
  for (int i = 0; i < num_accept_states; i++)
    fsa->final_states.insert(accept_states[i]);

  // Set the transition functions
  for (int i = 0; i < num_elements; i++) 
    fsa->transitions[make_pair(elements[i].state, elements[i].input_char)] = elements[i].next_state;
  return true;
}

bool BuildNFA(const TableElement* elements, int num_elements,
              const int* accept_states_array, int num_accept_states,
              FiniteStateAutomaton* fsa) {
  // Check the number of elements
  cout << "num_elements: " << num_elements << endl;
  if (num_elements <= 0) return false;

  fsa->type = NFA_DFA;

  // Set the initial state
  fsa->initial_state = elements[0].state;

  multimap<pair<int, char>, int> NFA_transitions;
  
  // Set the transition functions;
  for (int i = 0; i < num_elements; i++)
	NFA_transitions.insert(make_pair(make_pair(elements[i].state,
					  elements[i].input_char), elements[i].next_state));
  queue<set<int> > set_q;
  set<int> result_set;
  set< set<int> > check_dup_set;
  result_set.insert(fsa->initial_state);
  set_q.push(result_set);

  int len = sizeof(alphabet) / sizeof(alphabet[0]);
  // Convert NFA to DFA
  while (!set_q.empty()) {
	set<int> current_state_set = set_q.front();
	// Deal with lambda transition
	set<int> temp_set = current_state_set;
	temp_set = find_lambda_transition(temp_set, NFA_transitions);
    // Find next set of current state set
    pair<multimap<pair<int, char>, int>::iterator, multimap<pair<int, char>, int>::iterator> ret;
    for (int i = 0; i < len; i++) {
	  result_set.clear();
      for (set<int>::iterator set_it = temp_set.begin(); set_it != temp_set.end(); set_it++) {
        int state = *(set_it);
        ret = NFA_transitions.equal_range(make_pair(state, alphabet[i]));
        for (multimap<pair<int, char>, int>::iterator mul_it = ret.first; mul_it != ret.second; mul_it++)
		      result_set.insert(mul_it->second);
	  }
	  result_set = find_lambda_transition(result_set, NFA_transitions);
	  fsa->transitions_to_set[make_pair(current_state_set, alphabet[i])] = result_set;
	  if (!result_set.empty() && check_dup_set.insert(result_set).second == true)  set_q.push(result_set);
	  // Set final_states
	  for (int i = 0; i < num_accept_states; i++) {
		set<int> tmp_set = result_set;  
		// 
	   	if (tmp_set.insert(accept_states_array[i]).second == false)
	 	  fsa->final_states_sets.insert(result_set);
	  }
    }
    set_q.pop();
  }
  return true;
}

void print_set(const set<int>& _set)
{
  for (set<int>::iterator it = _set.begin(); it != _set.end(); it++)
	  cout << *(it);
  cout << endl;
}

set<int>& find_lambda_transition(set<int>& state_set, multimap<pair<int, char>, int>& NFA_transitions)
{
  pair<multimap<pair<int, char>, int>::iterator, multimap<pair<int, char>, int>::iterator> l_ret;
  for (set<int>::iterator set_it = state_set.begin(); set_it != state_set.end(); set_it++) {
    queue<int> lambda_q;
	lambda_q.push(*(set_it));
	while(!lambda_q.empty()) {
	  int l_state = lambda_q.front();
	  l_ret = NFA_transitions.equal_range(make_pair(l_state, '#'));
	  for (multimap<pair<int, char>, int>::iterator mul_it = l_ret.first; mul_it != l_ret.second; mul_it++) {
	    if (state_set.insert(mul_it->second).second == true)
	      lambda_q.push(mul_it->second);
	  }
	  lambda_q.pop();
	}
  }
  return state_set;
}
// Homework 1.1
bool RunFSA(const FiniteStateAutomaton* fsa, const char* str) {
  int current_state = fsa->initial_state;
  int i = 0;
  int length = strlen(str);
  bool undefined = false;
  if (fsa->type == DFA) {
	  map<pair<int, char>, int>::const_iterator it;
	  while (!undefined && i < length) {
		if ((it = fsa->transitions.find(make_pair(current_state, str[i])))
							!= fsa->transitions.end()) {
			current_state = it->second;
			i++;
		}
		else {
		  undefined = true;
		  return false;
		}
	  }
	  
	  if (fsa->final_states.find(current_state)
						!= fsa->final_states.end()) 
		return true;
	  else 
		return false;
   }
  else {
    map< pair<set<int>, char>, set<int> >::const_iterator it;
    set<int> current_states_set;
    current_states_set.insert(current_state);
    while (!undefined && i < length) {
      if ((it = fsa->transitions_to_set.find(make_pair(current_states_set, str[i])))
			  != fsa->transitions_to_set.end()) {
	    current_states_set = it->second;
	    i++;
	  }
	  else {
	      undefined = true;
		  return false;
	  }
	}
	
    for (set< set<int> >::const_iterator s_it = fsa->final_states_sets.begin();
			s_it != fsa->final_states_sets.end(); s_it++)
		if (current_states_set == *(s_it)) return true;
	return false;
  }
}

// Homework 1.1 and 1.2
bool BuildFSA(const TableElement* elements, int num_elements,
              const int* accept_states, int num_accepts,
              FiniteStateAutomaton* fsa) {
  if (CheckIfNFA(elements, num_elements)) {
    return BuildNFA(elements, num_elements, accept_states, num_accepts, fsa);
  } else {
    return BuildDFA(elements, num_elements, accept_states, num_accepts, fsa);
  }
}

// Homework 1.3
bool BuildFSA(const char* regex, FiniteStateAutomaton* fsa) {
  // Implement this function.
  return false;
}

