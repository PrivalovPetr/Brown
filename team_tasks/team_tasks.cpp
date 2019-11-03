﻿#include <tuple>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

#include "Header.h"

using namespace std;

TaskStatus Next(TaskStatus task_status) {
	return static_cast<TaskStatus>(static_cast<int>(task_status) + 1);
}

class TeamTasks {
private:
	map<string, TasksInfo> person_taskinfo;
public:
	// Получить статистику по статусам задач конкретного разработчика
	const TasksInfo& GetPersonTasksInfo(const string& person) const {
		if (person_taskinfo.count(person) > 0) {
			return person_taskinfo.at(person);
		}
		return TasksInfo();
	}

	// Добавить новую задачу (в статусе NEW) для конкретного разработчитка
	void AddNewTask(const string& person) {
		++person_taskinfo[person][TaskStatus::NEW];
	}
private:
	// Функция для удаления нулей из словаря
	void RemoveZeros(TasksInfo& tasks_info) {
		// Соберём те статусы, которые нужно убрать из словаря
		vector<TaskStatus> statuses_to_remove;
		for (const auto& task_item : tasks_info) {
			if (task_item.second == 0) {
				statuses_to_remove.push_back(task_item.first);
			}
		}
		for (const TaskStatus status : statuses_to_remove) {
			tasks_info.erase(status);
		}
	}
public:

	// Обновить статусы по данному количеству задач конкретного разработчика,
	// подробности см. ниже
	tuple<TasksInfo, TasksInfo> PerformPersonTasks(
		const string& person, int task_count) {

		TasksInfo& current = person_taskinfo[person];
		TasksInfo updated;
		TasksInfo forgiven;
		for (TaskStatus status = TaskStatus::NEW; status != TaskStatus::DONE; status = Next(status)) {
			updated[Next(status)] = min(current[status], task_count);
			task_count -= updated[Next(status)];
		}

		for (TaskStatus status = TaskStatus::NEW;
			status != TaskStatus::DONE;
			status = Next(status)) {
			forgiven[status] = current[status] - updated[Next(status)];
			current[status] += updated[status] - updated[Next(status)];
		}
		// По условию, DONE задачи не нужно возвращать в не обновлённых задачах
		current[TaskStatus::DONE] += updated[TaskStatus::DONE];

		// По условию в словарях не должно быть нулей
		RemoveZeros(updated);
		RemoveZeros(forgiven);

		return { updated, forgiven };
	}
};


void PrintTasksInfo(TasksInfo tasks_info) {
	cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
		", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
		", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
		", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
	TeamTasks tasks;
	tasks.AddNewTask("Ilia");
	for (int i = 0; i < 3; ++i) {
		tasks.AddNewTask("Ivan");
	}
	cout << "Ilia's tasks: ";
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
	cout << "Ivan's tasks: ";
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

	TasksInfo updated_tasks, untouched_tasks;

	tie(updated_tasks, untouched_tasks) =
		tasks.PerformPersonTasks("Ivan", 2);
	cout << "Updated Ivan's tasks: ";
	PrintTasksInfo(updated_tasks);
	cout << "Untouched Ivan's tasks: ";
	PrintTasksInfo(untouched_tasks);

	tie(updated_tasks, untouched_tasks) =
		tasks.PerformPersonTasks("Ivan", 2);
	cout << "Updated Ivan's tasks: ";
	PrintTasksInfo(updated_tasks);
	cout << "Untouched Ivan's tasks: ";
	PrintTasksInfo(untouched_tasks);

	return 0;
}
