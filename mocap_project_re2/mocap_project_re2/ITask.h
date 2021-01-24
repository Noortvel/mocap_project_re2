#pragma once

template<typename Input, typename Result>
class ITask {
protected:
	Result result;
public:
	virtual void Execute(Input& input) = 0;
	const virtual Result& Result() const{
		return result;
	}
};
