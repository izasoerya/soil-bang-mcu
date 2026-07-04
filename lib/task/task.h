#if !defined(TASK_H)
#define TASK_H

#include <vector>
#include <Arduino.h>

class Task
{
    long _delay = 0;

public:
    Task() {}
    virtual ~Task() {}

    virtual void init() = 0;
    virtual void loop() = 0;
    void setDelay(long delay) { _delay = delay; }
    long getDelay() { return _delay; }
};

class TaskRunner
{
private:
    std::vector<Task *> _taskList;
    std::vector<uint64_t> _prevTime;
    std::vector<bool> _firstRun;

    void _setRunner(Task *task, uint64_t *prevTime)
    {
        if (millis() - *prevTime > task->getDelay())
        {
            task->loop();
            *prevTime = millis();
        }
    }

public:
    TaskRunner() {}
    ~TaskRunner() {}

    void addTask(Task *task)
    {
        _taskList.push_back(task);
        _prevTime.push_back(0);
        _firstRun.push_back(true);
    }

    void loopTask()
    {
        for (int i = 0; i < _taskList.size(); i++)
        {
            if (_firstRun[i])
            {
                _taskList[i]->init();
                _firstRun[i] = false;
            }
            _setRunner(_taskList[i], &_prevTime[i]);
        }
    }
};

#endif // TASK_H
