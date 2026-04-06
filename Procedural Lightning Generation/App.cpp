#include "App.h"
#include "raylib.h"


void App::Run()
{
    InitWindow(1800, 900, "3D DBM Lightning");
    SetTargetFPS(60);

    imgui_controller.init();

    lightning.regenLightning();

    camera.position = Vector3({ 100.0f, 75.0f, -85.0f });  // Camera position
    camera.target = Vector3({ 0.0f, 0.0f, 0.0f });      // Camera looking at point
    camera.up = Vector3({ 0.0f, 1.0f, 0.0f });          // Camera up vector (rotation towards target)
    camera.fovy = 120.0f; // Camera field-of-view Y
    camera.projection = CAMERA_ORTHOGRAPHIC;             // Camera mode type


    while (!WindowShouldClose()) {
        Update();
        Render();
    }

    CloseWindow();
}

void App::Update()
{
    std::chrono::milliseconds duration = std::chrono::milliseconds();

    if (lightning_config.is_perform_test) // i still really dont like looping through this many times but what can you do
    {
        TestData this_test;


        if (lightning_config.test_type == TEST_TYPE::time_test)
        {
            std::ofstream timing_file("times.txt");
            lightning_config.times.clear();

            for (int x = 5; x <= lightning_config.max_dimension; x += lightning_config.dimension_increment)
            {
                for (int y = 5; y <= lightning_config.max_dimension; y += lightning_config.dimension_increment)
                {
                    for (int z = 5; z <= lightning_config.max_dimension; z += lightning_config.dimension_increment)
                    {

                        lightning_config.times.clear();
                        lightning_config.max_time = std::numeric_limits<float>::min();
                        lightning_config.min_time = std::numeric_limits<float>::max();


                        for (int i = 0; i < lightning_config.number_to_average; i++) // do five tests at each dimension
                        {
                            lightning_config.x_size = x;
                            lightning_config.y_size = y;
                            lightning_config.z_size = z;
                            auto time_at_start = std::chrono::high_resolution_clock::now();

                            lightning.regenLightning();
                            auto time_at_end = std::chrono::high_resolution_clock::now();
                            duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start);


                            lightning_config.times.push_back((float)duration.count());
                            lightning_config.max_time = std::max(lightning_config.max_time, (float)duration.count());
                            lightning_config.min_time = std::min(lightning_config.min_time, (float)duration.count());

                            Render(); // very cheeky, means we can watch it in real time, doesnt affect timing
                        }

                        float average_time = 0;

                        for (int i = 0; i < lightning_config.number_to_average; i++)
                        {
                            average_time += lightning_config.times[i];
                        }
                        average_time /= lightning_config.number_to_average;

                        timing_file << x << "," << y << "," << z << "," << std::fixed << average_time << "," << std::fixed << lightning_config.min_time << "," << std::fixed << lightning_config.max_time << "\n";

                        lightning_config.xs.push_back(x);
                        lightning_config.ys.push_back(y);
                        lightning_config.zs.push_back(z);

                        lightning_config.avg_times.push_back(average_time); // log time as number gets big
                        lightning_config.min_times.push_back(lightning_config.min_time);
                        lightning_config.max_times.push_back(lightning_config.max_time);
                    }
                }
            }

            timing_file.close();
        }
        else if (lightning_config.test_type == TEST_TYPE::target_test)
        {
            auto points = lightning.getLightningPointsPtr();
            for (int i = 0; i < lightning_config.targets_to_test; i++)
            {
                lightning.regenLightning();
                this_test.target_results.struck_cells[index(points->back().x, points->back().z)]++;
                Render();
            }
        }
        lightning_config.is_perform_test = false;
        tests.push_back(this_test);
    }
    else if (lightning_config.is_regenerate_this_frame)
    {
        auto time_at_start = std::chrono::high_resolution_clock::now();

        lightning.regenLightning();

        auto time_at_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start);

        SavedGeneration temp;
        temp.x_size = lightning_config.x_size;
        temp.y_size = lightning_config.y_size;
        temp.z_size = lightning_config.z_size;
        temp.eta = lightning_config.eta;
        temp.time = duration.count();
        temp.grid_steps = lightning_config.grid_steps;
        temp.candidates_from_air = lightning_config.candidates_from_air;
        temp.multithreading_enabled = lightning_config.is_multithread;
        temp.resetting_volume = lightning_config.reset_vol_between_steps;
        temp.gradient_tolerance = lightning_config.gradient_tolerance;
        temp.loop_cap_enabled = lightning_config.use_loop_cap;
        temp.max_loops = lightning_config.max_laplace_loops;


        lightning_config.saved_info.push_back(temp);

        std::cout << "Time Taken: " << duration.count() <<"ms" << std::endl;

        lightning_config.is_regenerate_this_frame = false;
    }
}

void App::Render()
{
    BeginDrawing();
    ClearBackground(BLACK);


    renderer.render();

    imgui_controller.render(lightning_config);

    EndDrawing();
}

