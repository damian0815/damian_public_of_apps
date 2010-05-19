/*
 *  AnimStateMachine.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"
class Light;

class AnimStateMachine: public Animation
{
public:
	AnimStateMachine( Lights* _lights );
	
	static const char* NAME;

	void update( float elapsed );
	void draw();

private:
	
	void loadTransitions();
	
	typedef enum _State 
	{
		STATE_IDLE,
		STATE_BUILDUP,
		STATE_BLIP,
		STATE_RELEASE
	} State ;
	State current_state;
	float timer;
	
	// returns true when it's time to get a new state
	bool updateState( float elapsed );
	void enterState( State new_state );
	void nextState();
	
	typedef struct StateTransitionProbability
	{
		State next_state;
		float probability;
		StateTransitionProbability( State s, float p ) { next_state = s; probability = p; }
		StateTransitionProbability() { next_state = STATE_IDLE; probability = 1; }
	} StateTransitionProbability;
	typedef map< State, vector<StateTransitionProbability> > Transitions;
	Transitions transitions;

	// return a squared random number between first and second inclusive, favouring first
	float getSquaredRandom( float first, float second );
	

};