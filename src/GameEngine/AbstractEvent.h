class StateSnapshot;

class AbstractEvent {
public:
	virtual void execute(StateSnapshot state);
	virtual int operator<(const AbstractEvent& other) const = 0;
};