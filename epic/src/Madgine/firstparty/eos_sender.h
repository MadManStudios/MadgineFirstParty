#pragma once

#include "Generic/execution/sender.h"
#include "Generic/genericresult.h"

namespace Engine {
	namespace FirstParty {

		template <typename R>
		R infoTypeHelper(void (*)(const R*));

		template <typename F>
		using InfoTypeHelper = decltype(infoTypeHelper(std::declval<F>()));

		template <auto f, typename R, typename Rec, typename... Args>
		struct eos_state {
			void start()
			{
				TupleUnpacker::invokeFromTuple([this](Args... args) {
					f(std::forward<Args>(args)..., this, &eos_state::callback);
					},
					std::move(mArgs));
			}
			void stop()
			{
				throw 0;
			}
			static void callback(const R* result)
			{
				eos_state* self = static_cast<eos_state*>(result->ClientData);
				self->mRec.set_value(*result);
			}
			Rec mRec;
			std::tuple<Args...> mArgs;
		};

		template <auto f, typename... Args>
		struct EOS_senderImpl
		{
			EOS_senderImpl(Args &&...args) :
				mArgs(std::forward<Args>(args)...) {

			}

			using is_sender = void;

			using argument_types = typename Callable<f>::traits::argument_types;
			using C = typename argument_types::template select<argument_types::size - 1>;

			using result_type = GenericResult;
			template <template <typename...> typename Tuple>
			using value_types = Tuple<InfoTypeHelper<C>>;

			template <typename Rec>
			friend auto tag_invoke(Execution::connect_t, EOS_senderImpl&& sender, Rec&& rec) {
				return eos_state<f, InfoTypeHelper<C>, Rec, Args...> {std::forward<Rec>(rec), std::move(sender.mArgs)};
			}

			std::tuple<Args...> mArgs;
		};

		template <auto f, typename... Args>
		auto EOS_sender(Args &&... args) {
			return EOS_senderImpl<f, Args...>{std::forward<Args>(args)...};
		}
	}
}