// PL homework: hw2
// lr_parser.cc

#include <assert.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <stack>
#include <cstring>
#include "lr_parser.h"

#define DISABLE_LOG false
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

using namespace std;

bool BuildLRParser(const LRTableElement* elements, int num_elements,
                   const LRRule* rules, int num_rules,
                   LRParser* lr_parser) {

  if (num_elements <= 0 || num_rules <= 0) return false;
  if (elements == NULL || rules == NULL || lr_parser == NULL) return false;

  for (int i = 0; i < num_elements; i++) {
	pair<int, int> key = make_pair(elements[i].state, elements[i].symbol);
	pair<LRAction, int> value = make_pair(elements[i].action, elements[i].next_state);
    lr_parser->p_table.insert(make_pair(key, value));
  }

  for (int j = 0; j < num_rules; j++)
	lr_parser->p_rules.push_back(rules[j]);
  
  return true;
}

bool RunLRParser(const LRParser* lr_parser, const char* str) {
  // Init the stack
  stack<int> p_stack;
  p_stack.push(0);

  // Begin LR parsing
  int i = 0;
  int str_length = strlen(str);

  while (i < str_length) {
	pair<LRAction, int> action_state_pair;  
	try {
	  action_state_pair = lr_parser->p_table.at(make_pair(p_stack.top(), str[i]));
	}
 	catch (const out_of_range& orr) {// str에 정의되지 않은 입력이 있을 경우
      cerr << "올바른 식을 입력해주세요." << endl;
	  break;
	}


	if (action_state_pair.first == SHIFT) {
	  p_stack.push(action_state_pair.first);
	  p_stack.push(action_state_pair.second);
	  i++;
	}
	else if (action_state_pair.first == REDUCE) {
	  for (int pop_count = 0; 
			  pop_count < lr_parser->p_rules[action_state_pair.second - 1].num_rhs * 2;
			  pop_count++) {
	    p_stack.pop(); 
	  }

	  int lhs_symbol = lr_parser->p_rules[action_state_pair.second - 1].lhs_symbol;
	  int next_state = lr_parser->p_table.at(make_pair(p_stack.top(), lhs_symbol)).second;
	  
	  p_stack.push(lhs_symbol);
      p_stack.push(next_state);
	}
	else if (action_state_pair.first == ACCEPT) {
	  return true;
	}
	else if (action_state_pair.first == INVALID) {
	  cerr << "Invalid Action" << endl;
	}
  }
  return false;
}

