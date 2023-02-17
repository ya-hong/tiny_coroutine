#pragma once

#include <cassert>

namespace tiny_coroutine {

namespace detail {

enum promise_state {
	Pregnancy,	  // no result
	Detach,		  // no result & do not care result
	Birth,		  // has result and not read
	RePregnancy,  // has result and read (prepare for generate new result)
	Abort,		  //
				  // Abandon,	  		// need deconstruct
};

/**
 *   Detach -----------------------------|
 *     ↑                                 |
 *     |                                 |
 * Pregnancy  ---------------------------|
 * |   |                                 |
 * |   ↓                                 ↓
 * | Birth ----------------------------> Abort
 * |  ↑  |                               ↑
 * ↓  |  ↓                               |
 *  RePregnancy -------------------------|
 *
 *
 */

// void transfer_write(promise_state &state) {
// 	assert(state != promise_state::ABORTION &&
// 		   state != promise_state::ABANDON && "coroutine canceled");
// 	state = promise_state::BIRTH;
// }

// void transfer_read(promise_state &state) {
// 	assert(state == promise_state::BIRTH && "await not ready");
// 	state = promise_state::REPREGNANCY;
// }

// void transfer_cancel(promise_state &state) {
// 	switch (state) {
// 	case promise_state::Pregnancy:
// 	case promise_state::DETACH_PREGNANCY:
// 		state = promise_state::ABORTION;
// 		break;
// 	case promise_state::BIRTH:
// 	case promise_state::REPREGNANCY:
// 		state = promise_state::ABANDON;
// 		break;
// 	default:
// 		break;
// 	}
// }

}  // namespace detail

}  // namespace tiny_coroutine
