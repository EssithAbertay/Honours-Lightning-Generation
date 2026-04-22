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
  
    if (lightning_config.is_perform_test) // if testing
    { 
        if (lightning_config.test_type == TEST_TYPE::time_test) // what type of test
        {     
            std::ofstream timing_file(filename("times","Results/Generation",".json")); //create the file
            nlohmann::ordered_json jsonfile;

            // write setup info to file
            jsonfile["testype"] = "Timing";
            jsonfile["setup"] = lightning_config.to_json();
            jsonfile["setup"]["eta"] = "N/A";

            
            nlohmann::ordered_json results = nlohmann::json::array();

            int number_of_tests = lightning_config.times_to_test;

            // run tests, testing times at every eta value
            for (int eta_test = 1; eta_test <= lightning_config.max_eta; eta_test++)
            {
                lightning_config.eta = eta_test;

                nlohmann::ordered_json result;
                result["eta"] = eta_test;

             
                float max_time = std::numeric_limits<float>::lowest();
                float min_time = std::numeric_limits<float>::max();
                std::vector<float> times;
                float average_time = 0;

                int maximum_segment_count = std::numeric_limits<int>::lowest();
                int minimum_segment_count = std::numeric_limits<int>::max();
                std::vector<int> segment_counts;
                float average_segment_count = 0;

                int maximum_grid_steps = std::numeric_limits<int>::lowest();
                int minimum_grid_steps = std::numeric_limits<int>::max();
                std::vector<int> grid_steps;
                float average_grid_steps = 0;

                auto points = lightning.getLightningPointsPtr();

                for (int i = 0; i < number_of_tests; i++)
                {
                    // the test itself
                    auto time_at_start = std::chrono::high_resolution_clock::now(); 
                    lightning.regenLightning();
                    auto time_at_end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start); // find time taken

                    float time = duration.count();   
                    
                    // gather data

                    int num_of_segments = points->size();
                    int num_of_grid_steps = lightning_config.grid_steps;

                    times.push_back(time);
                    segment_counts.push_back(num_of_segments);
                    grid_steps.push_back(num_of_grid_steps);

                    max_time = std::max(max_time, time);
                    min_time = std::min(min_time, time);
                   
                    maximum_segment_count = std::max(num_of_segments, maximum_segment_count);
                    minimum_segment_count = std::min(num_of_segments, minimum_segment_count);

                    maximum_grid_steps = std::max(num_of_grid_steps, maximum_grid_steps);
                    minimum_grid_steps = std::min(num_of_grid_steps, minimum_grid_steps);


                    average_time += time;
                    average_segment_count += num_of_segments;
                    average_grid_steps += num_of_grid_steps;

                    if (lightning_config.render_during_test)
                    {
                        Render(); // very cheeky, means we can watch it in real time, doesnt affect timing
                    }
                }

                // finding averages
                average_time /= number_of_tests;
                average_segment_count /= number_of_tests;
                average_grid_steps /= number_of_tests;

                // adding all data to the json

                result["average time"] = average_time;
                result["minimum time"] = min_time;
                result["maximum time"] = max_time;

                nlohmann::json all_times = nlohmann::json::array();

                for (auto t : times)
                {
                    all_times.push_back(t);
                }
                
                result["times"] = all_times;

                result["average segment count"] = average_segment_count;
                result["minimum segment count"] = minimum_segment_count;
                result["maximum segment count"] = maximum_segment_count;

                nlohmann::json all_segment_counts = nlohmann::json::array();

                for (auto sc : segment_counts)
                {
                    all_segment_counts.push_back(sc);
                }

                result["segment counts"] = all_segment_counts;

                result["average grid steps"] = average_grid_steps;
                result["minimum grid steps"] = minimum_grid_steps;
                result["maximum grid steps"] = maximum_grid_steps;

                nlohmann::json all_grid_steps = nlohmann::json::array();

                for (auto gs : grid_steps)
                {
                    all_grid_steps.push_back(gs);
                }

                result["grid steps"] = all_grid_steps;


                results.push_back(result);
            }

            //dumps the json into the file

            jsonfile["results"] = results;


            timing_file << jsonfile.dump(4);
            timing_file.close();
        }
        else if (lightning_config.test_type == TEST_TYPE::target_test)
        {
            std::ofstream target_file(filename("targets", "Results/Targets", ".json"));   

            std::map<int, int> struck_cells;

            nlohmann::ordered_json jsonfile;

            //write setup info
            jsonfile["testype"] = "Targeting";
            jsonfile["setup"] = lightning_config.to_json();

         
            // run tests
            auto points = lightning.getLightningPointsPtr();
            for (int i = 0; i < lightning_config.targets_to_test; i++)
            {
                lightning.regenLightning();
                struck_cells[index(points->back().x, points->back().z)]++;

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
                        {"strikes",struck_cells[index(x, z)]},
                        });
                }
            }

            jsonfile["results"] = results;
            // write to json
            target_file << jsonfile.dump(4);
            target_file.close();
        }
        lightning_config.is_perform_test = false;
    }
    else if (lightning_config.is_regenerate_this_frame) // if just generating a new lighting
    {
        auto time_at_start = std::chrono::high_resolution_clock::now();

        lightning.regenLightning();

        auto time_at_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_at_end - time_at_start);

        TestData this_test;

        // save the info from this generation for later display

        this_test.conditions = lightning_config.getSetup();
        this_test.time = duration.count();
        this_test.grid_steps = lightning_config.grid_steps;
        this_test.number_of_segments = lightning.getLightningPointsPtr()->size();

        tests.push_back(this_test);

        lightning_config.is_regenerate_this_frame = false;
    }
}

void App::Render() // render everything!
{
    BeginDrawing();
    ClearBackground(BLACK);

    renderer.render();

    imgui_controller.render(lightning_config, tests);

    EndDrawing();
}

