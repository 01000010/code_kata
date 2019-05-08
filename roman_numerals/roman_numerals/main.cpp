#include <iostream>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <stack>
#include <vector>
#include <algorithm>
#include <string>
#include <string_view>
#include <fstream>

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
};

class Revertable
{
public:
    virtual ~Revertable() = default;

    virtual void undo() = 0;
};

class UndoableCommand : public Command, public Revertable {};

using CommandPtr = std::shared_ptr<UndoableCommand>;

class HelloWorldOutputCommand : public Command
{
public:
    virtual void execute() override
    {
        std::cout << "Hello, world!" << std::endl;
    }
};

class Point final
{
private:
    int x;
    int y;

public:
    Point() = default;
    Point(int x, int y) : x{ x }, y{ y } {}

    int get_x() const noexcept { return x; }
    int get_y() const noexcept { return y; }
};

class DrawingProcessor
{
public:
    void draw_circle(const Point& center_point, const double radius)
    {
        std::cout << "Drew a circle at point (" << center_point.get_x() << ',' << center_point.get_y()
            << ") with radius " << radius << std::endl;
    }

    void erase_circle(const Point& center_point, const double raidus)
    {
        std::cout << "Erased a circle at point (" << center_point.get_x() << ',' << center_point.get_y()
            << ") with radius " << raidus << std::endl;
    }

    void draw_rectangle(const Point& center_point, const double height, const double width)
    {
        std::cout << "Drew a rectangle at point (" << center_point.get_x() << ',' << center_point.get_y()
            << ") with height " << height << " and width " << width << std::endl;
    }

    void erase_rectangle(const Point& center_point, const double height, const double width)
    {
        std::cout << "Erased a rectangle at point (" << center_point.get_x() << ',' << center_point.get_y()
            << ") with height " << height << " and width " << width << std::endl;
    }
};

class DrawCircleCommand : public UndoableCommand
{
private:
    DrawingProcessor& reciever;
    const Point center_point;
    const double radius;

public:
    DrawCircleCommand(DrawingProcessor& reciever, const Point& center_point, const double radius) noexcept
        : reciever{ reciever }, center_point{ center_point }, radius{ radius }
    {}

    virtual void execute() override
    {
        reciever.draw_circle(center_point, radius);
    }

    virtual void undo() override
    {
        reciever.erase_circle(center_point, radius);
    }
};

class DrawRectangleCommand : public UndoableCommand
{
private:
    DrawingProcessor& reciever;
    const Point center_point;
    const double height;
    const double width;

public:
    DrawRectangleCommand(DrawingProcessor& reciever, const Point& center_point, const double height,
        const double width)
        : reciever{ reciever }, center_point{ center_point }, height{ height }, width{ width }
    {}

    virtual void execute() override
    {
        reciever.draw_rectangle(center_point, height, width);
    }

    virtual void undo() override
    {
        reciever.erase_rectangle(center_point, height, width);
    }
};

class CommandProcessor
{
private:
    std::stack<std::shared_ptr<Revertable>> command_history;

public:
    void execute(const CommandPtr& command)
    {
        command->execute();
        command_history.push(command);
    }

    void undo_last_command()
    {
        if (command_history.empty()) {
            return;
        }

        command_history.top()->undo();
        command_history.pop();
    }
};

class CompositeCommand : public UndoableCommand
{
private:
    std::vector<CommandPtr> commands;

public:
    void add_command(CommandPtr& command)
    {
        commands.push_back(command);
    }

    virtual void execute() override
    {
        for (const auto& command : commands) {
            command->execute();
        }
    }

    virtual void undo() override
    {
        for (const auto& command : commands) {
            command->undo();
        }
    }
};

class UndoCommand : public UndoableCommand
{
private:
    CommandProcessor& reciever;

public:
    explicit UndoCommand(CommandProcessor& reciever) noexcept
        : reciever{ reciever }
    {}

    virtual void execute() override
    {
        reciever.undo_last_command();
    }

    virtual void undo() override
    {
        // Leave empty for now, but in case we could undo the last undo... hrmmmmm
    }
};

class Server
{
public:
    void accept_command(const CommandPtr& command)
    {
        command->execute();
    }
};

class WaitCommand : public Command
{
private:
    unsigned int duration_milliseconds{ 1000 };

public:
    explicit WaitCommand(const unsigned int duration_milliseconds) noexcept
        : duration_milliseconds{ duration_milliseconds }
    {}

    virtual void execute() override
    {
        std::chrono::milliseconds duration{ duration_milliseconds };
        std::this_thread::sleep_for(duration);
    }
};

// class Client
// {
// public:
//     void run()
//     {
//         Server server{};
//         const unsigned int SERVER_DELAY_TIMESPAN{ 3000 };
//         CommandPtr wait_command{ std::make_shared<WaitCommand>(SERVER_DELAY_TIMESPAN) };
//         CommandPtr hello_world_command{ std::make_shared<HelloWorldOutputCommand>() };
// 
//         server.accept_command(wait_command);
//         server.accept_command(hello_world_command);
//     }
// };

class Observer
{
public:
    virtual ~Observer() = default;
    virtual int get_id() = 0;
    virtual void update() = 0;
};

using ObserverPtr = std::shared_ptr<Observer>;

class IsEqualTo final
{
private:
    ObserverPtr observer;

public:
    explicit IsEqualTo(const ObserverPtr& observer) : observer{ observer } {}

    bool operator()(const ObserverPtr& to_compare)
    {
        return to_compare->get_id() == observer->get_id();
    }
};

class Subject
{
private:
    std::vector<ObserverPtr> observers;

protected:
    void notify_all_observers() const
    {
        for (const auto& observer : observers) {
            observer->update();
        }
    }

public:
    void add_observer(ObserverPtr& new_observer)
    {
        auto iterator{ std::find_if(std::begin(observers), std::end(observers), IsEqualTo{new_observer}) };

        if (iterator == std::end(observers)) {
            observers.push_back(new_observer);
        }
    }

    void remove_observer(ObserverPtr& observer_to_remove)
    {
        observers.erase(
            std::remove_if(
                std::begin(observers), std::end(observers), IsEqualTo{ observer_to_remove }
            ), std::end(observers)
        );
    }
};

class SpreadsheetModel : public Subject
{
public:
    void change_cell_value(std::string_view column, const int row, const double value)
    {
        std::cout << "Cell [" << column << ", " << row << "] = " << value << std::endl;
        notify_all_observers();
    }
};

class TableView : public Observer
{
private:
    SpreadsheetModel& model;

public:
    explicit TableView(SpreadsheetModel& the_model) : model{ the_model } {}

    virtual int get_id() override
    {
        return 1;
    }

    virtual void update() override
    {
        std::cout << "Update of TableView." << std::endl;
    }
};

class BarChartView : public Observer
{
private:
    SpreadsheetModel& model;

public:
    explicit BarChartView(SpreadsheetModel& the_model) : model{ the_model } {}

    virtual int get_id() override
    {
        return 2;
    }

    virtual void update() override
    {
        std::cout << "Update of BarChartView." << std::endl;
    }
};

class PieChartView : public Observer
{
private:
    SpreadsheetModel& model;

public:
    explicit PieChartView(SpreadsheetModel& the_model) : model{ the_model } {}

    virtual int get_id() override
    {
        return 3;
    }

    virtual void update() override
    {
        std::cout << "Update of PieChartView." << std::endl;
    }
};

int main()
{
    // SpreadsheetModel spreadsheet_model{};
	// 
    // ObserverPtr tableview_observer{ std::make_shared<TableView>(spreadsheet_model) };
	// 
    // spreadsheet_model.add_observer(tableview_observer);
	// 
    // ObserverPtr barchart_observer{ std::make_shared<BarChartView>(spreadsheet_model) };
	// 
    // spreadsheet_model.add_observer(barchart_observer);
    // spreadsheet_model.change_cell_value("A", 1, 42);
    // spreadsheet_model.remove_observer(tableview_observer);
    // spreadsheet_model.change_cell_value("B", 2, 23.1);
	// 
    // ObserverPtr piechart_observer{ std::make_shared<PieChartView>(spreadsheet_model) };
	// 
    // spreadsheet_model.add_observer(piechart_observer);
    // spreadsheet_model.change_cell_value("C", 3, 3.1415926);

    // CommandProcessor command_processor{};
    // DrawingProcessor drawing_processor{};
    // std::shared_ptr<CompositeCommand> macro_recorder{ std::make_unique<CompositeCommand>() };
    // Point circle_center_point{ 20, 20 };
    // CommandPtr draw_circle_command{
    //     std::make_shared<DrawCircleCommand>(drawing_processor, circle_center_point, 10)
    // };
    // 
    // command_processor.execute(draw_circle_command);
    // macro_recorder->add_command(draw_circle_command);
    // 
    // Point rectangle_center_point{ 30, 10 };
    // CommandPtr draw_rectangle_command{
    //     std::make_shared<DrawRectangleCommand>(drawing_processor, rectangle_center_point, 5, 8)
    // };
    // 
    // command_processor.execute(draw_rectangle_command);
    // macro_recorder->add_command(draw_rectangle_command);
    // command_processor.execute(macro_recorder);
    // 
    // CommandPtr undo_command{ std::make_shared<UndoCommand>(command_processor) };
    // 
    // command_processor.execute(undo_command);

    // Client client{};
    // 
    // client.run();

    return 0;
}