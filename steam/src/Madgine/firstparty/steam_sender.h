#pragma once

#include "Generic/execution/sender.h"
#include "Generic/genericresult.h"

namespace Engine {
	namespace FirstParty {

		template <typename R, typename Rec>
		struct steam_state {
			void start()
			{
				mCallResult.Set(mCall, this, &steam_state::callback);
			}
			void stop()
			{
				mCallResult.Cancel();
				mRec.set_done();
			}
			void callback(R* result, bool bIOFailure)
			{
				if (bIOFailure) {
					mRec.set_error(GenericResult::UNKNOWN_ERROR);
				}
				else {
					mRec.set_value(*result);
				}
			}
			Rec mRec;
			SteamAPICall_t mCall;
			CCallResult<steam_state, R> mCallResult;
		};

		template <typename R>
		struct steam_sender
		{
			steam_sender(SteamAPICall_t call) :
				mCall(call)
			{
			}

			using is_sender = void;

			using result_type = GenericResult;
			template <template <typename...> typename Tuple>
			using value_types = Tuple<R>;

			template <typename Rec>
			friend auto tag_invoke(Execution::connect_t, steam_sender&& sender, Rec&& rec) {
				return steam_state<R, Rec> {std::forward<Rec>(rec), sender.mCall};
			}

			SteamAPICall_t mCall;
		};

	}
}