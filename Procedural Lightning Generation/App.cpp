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
            std::ofstream timing_file(filename("times","Results",".json"));

            nlohmann::ordered_json jsonfile;
            jsonfile["testype"] = "Targeting";
            jsonfile["setup"] = lightning_config.to_json();


 /*           timing_file << "Timing Test" << "\n";
            lightning_config.writeSetupToFile(timing_file);
            lightning_config.times.clear();*/


            // i think this should be local during the test rather than use config
            lightning_config.times.clear();
            lightning_config.max_time = std::numeric_limits<float>::min();
            lightning_config.min_time = std::numeric_limits<float>::max();

            int total_segment_count = 0;
            int minimum_segment_count = std::numeric_limits<float>::max();
            int maximum_segment_count = std::numeric_limits<int>::min();

            auto points = lightning.getLightningPointsPtr();

            // run tests
            for (int i = 0; i < lightning_config.times_to_test; i++) 
            {
                auto time_at_start = std::chrono::high_resolution_clock::now();

                lightning.regenLightning();
                auto time_at_end = std::chrono::high_resolution_clock::now();

                duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start);

                // store time from each test
                lightning_config.times.push_back((float)duration.count());
                lightning_config.max_time = std::max(lightning_config.max_time, (float)duration.count());
                lightning_config.min_time = std::min(lightning_config.min_time, (float)duration.count());


                total_segment_count += points->size();
                maximum_segment_count = std::max((int)points->size(), maximum_segment_count);
                minimum_segment_count = std::min((int)points->size(), minimum_segment_count);


                if (lightning_config.render_during_test)
                {
                    Render(); // very cheeky, means we can watch it in real time, doesnt affect timing
                }
            }

            // finding average
            float average_time = 0;
            float average_segment_count;

            for (int i = 0; i < lightning_config.times_to_test; i++)
            {
                average_time += lightning_config.times[i];
            }
            average_time /= lightning_config.times_to_test;
            average_segment_count = total_segment_count / lightning_config.times_to_test;


            jsonfile["average time (ms)"] = average_time;
            jsonfile["maximum time (ms)"] = lightning_config.min_time;
            jsonfile["minimum time (ms)"] = lightning_config.max_time;
            
            jsonfile["average segment count"] = average_segment_count;
            jsonfile["maximum segment count"] = minimum_segment_count;
            jsonfile["minimum segment count"] = maximum_segment_count;


            lightning_config.avg_times.push_back(average_time);
            lightning_config.min_times.push_back(lightning_config.min_time);
            lightning_config.max_times.push_back(lightning_config.max_time);

            timing_file << jsonfile.dump(4);
            timing_file.close();
        }
        else if (lightning_config.test_type == TEST_TYPE::target_test)
        {
            std::ofstream target_file(filename("targets", "Results", ".json"));   
            
            nlohmann::ordered_json jsonfile;

            jsonfile["testype"] = "Targeting";
            jsonfile["setup"] = lightning_config.to_json();

         
            // run tests
            auto points = lightning.getLightningPointsPtr();
            for (int i = 0; i < lightning_config.targets_to_test; i++)
            {
                lightning.regenLightning();
                this_test.target_results.struck_cells[index(points->back().x, points->back().z)]++;

                if (lightning_config.render_during_test)
                {
                    Render();
                }
            }

            // write test results
            nlohmann::json results = nlohmann::json::array();
            for (int z = 0; z < lightning_config.z_size; z++)
            {
                for (int x = 0; x < lightning_config.x_size; x++)
                {
                    results.push_back({
                        {"x",x},
                        {"z",z},
                        {"strikes",this_test.target_results.struck_cells[index(x, z)]},
                        });
                }
            }

            jsonfile["results"] = results;
            // write to json
            target_file << jsonfile.dump(4);
            target_file.close();
        }
        lightning_config.is_perform_test = false;
        
        SavedGeneration this_test_setup = lightning_config.getSetup(); // this is done at the end so as to get times? idk why anymore
        this_test.conditions.setup = this_test_setup;
        //todo add target

        tests.push_back(this_test);
    }
    else if (lightning_config.is_regenerate_this_frame)
    {
        auto time_at_start = std::chrono::high_resolution_clock::now();

        lightning.regenLightning();

        auto time_at_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start);

        SavedGeneration temp = lightning_config.getSetup();
        temp.time = duration.count();

        //todo add target

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

