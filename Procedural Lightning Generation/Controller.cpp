#include "Controller.h"


void Controller::init(bool dark_mode)
{
    rlImGuiSetup(dark_mode);
}

void Controller::render(Config & configuration, std::vector<TestData>& tests)
{
	// Dear ImGui controls
	// https://github.com/ocornut/imgui
	// ui panels that allow user control over all parameters in the program
	// also has panel for testing
	// and panel with table of results and setup info

    rlImGuiBegin();

	ImGui::Begin("Controls", NULL);

		if (ImGui::TreeNode("Camera Controls"))
		{
			static int method_val = 0;

			ImGui::RadioButton("Rotating", &method_val, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Free", &method_val, 1);

			configuration.cam_method = static_cast<CAMERA_METHOD>(method_val);

			if (method_val == 1)
			{
				ImGui::SliderInt("Camera Angle", &configuration.cam_angle, 0, 360);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Renderer Controls"))
		{
			ImGui::Checkbox("Line Mode", &configuration.line_mode);

			ImGui::SeparatorText("Capsule Controls");
			ImGui::SliderFloat("Radius", &configuration.radius, 0, 1);
			ImGui::SliderInt("Slices", &configuration.slices, 1, 10);
			ImGui::SliderInt("Rings", &configuration.rings, 1, 10);
			

			ImGui::SeparatorText("Other Renderer Options");
			ImGui::Checkbox("Wireframe Mode", &configuration.wireframe);
			ImGui::Checkbox("Display Bounding Box", &configuration.is_bounding_box);
			ImGui::Checkbox("Display Initial Charges", &configuration.is_initial_charge); // todo: fix
		
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Volume Controls"))
		{
			ImGui::SliderInt("X Size", &configuration.x_size, 5, 100);
			ImGui::SliderInt("Y Size", &configuration.y_size, 5, 100);
			ImGui::SliderInt("Z Size", &configuration.z_size, 5, 100);

			ImGui::Checkbox("Force 1:2:1 Volume ratio", &configuration.force_ratio);	
			
			if (configuration.force_ratio)
			{
				int new_size = std::max(configuration.y_size / 2, 5);
				configuration.x_size = new_size;
				configuration.z_size = new_size;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Generation Controls"))
		{	
			ImGui::SeparatorText("General");

			ImGui::SliderInt("Eta", &configuration.eta, 1, configuration.max_eta);	

		//	ImGui::Checkbox("Include border cells in potential calculation", &configuration.include_border_cells); // this caused weird stuff, user can no longer change it

			ImGui::Checkbox("Use Target", &configuration.use_target);

			if (configuration.use_target)
			{

				ImGui::SliderInt("Target X", &configuration.target_x, 0, configuration.x_size );
				ImGui::SliderInt("Target Z", &configuration.target_z, 0, configuration.z_size);

				configuration.target_y = configuration.y_size;

				ImGui::SliderFloat("Weighting", &configuration.target_lambda, 0, 0.1);
			}


			ImGui::SeparatorText("Optimisations");

			ImGui::Checkbox("Reset volume between steps", &configuration.reset_vol_between_steps);

	
			ImGui::Text("Select candidate cells from:");
			ImGui::RadioButton("Air cells", &configuration.candidate_selection, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Lightning cells", &configuration.candidate_selection, 1);

			// todo: could just be a 0/1 true/false, but switch allows for potential future options
			switch (configuration.candidate_selection)
			{
			case 0:
				configuration.candidates_from_air = true;
				break;
			case 1:
				configuration.candidates_from_air = false;
				break;
			default:
				break;
			}

			ImGui::Checkbox("Multithread", &configuration.is_multithread);

			if (ImGui::TreeNode("Gradient Calculation"))
			{
				ImGui::SliderFloat("Tolerance", &configuration.gradient_tolerance, 0, 1, "%.3f");

				ImGui::Checkbox("Use loop cap", &configuration.use_loop_cap);

				if (configuration.use_loop_cap)
				{
					ImGui::Checkbox("Use max loop calculation", &configuration.use_calculated_loops);

					if (configuration.use_calculated_loops)
					{
						ImGui::Text("Max loops is calculated as (y_size * x) where x is the multiplier value");
						ImGui::SliderFloat("Loop Multiplier", &configuration.loop_multiplier, 0.1f, 3, "%.1f");
					}
					else
					{
						ImGui::InputInt("Max Loops", &configuration.max_laplace_loops);
					}
				}

				ImGui::TreePop();
			}
		
			ImGui::TreePop();
		}
		

		if (ImGui::Button("Regenerate Lightning"))
		{
			configuration.is_regenerate_this_frame = true;
		}

		if (ImGui::Button("Restore Defaults"))
		{
			configuration = config_defaults; // todo: bug where defaults only reset when trees are open
		}

	ImGui::End();

	ImGui::Begin("Testing", NULL);

		ImGui::SeparatorText("Generation test controls");
		ImGui::InputInt("Number of runs", &configuration.times_to_test, 1, 10);

		ImGui::SeparatorText("Target test controls");
		ImGui::InputInt("Number of strikes to find", &configuration.targets_to_test, 1, 10);


		ImGui::Text("Test Type");
		static int e = 0;
		ImGui::RadioButton("Timing Test", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Target Test", &e, 1); 

		ImGui::Checkbox("Render during test", &configuration.render_during_test);

		if (ImGui::Button("Perform Test"))
		{
			configuration.is_perform_test = true;
			configuration.is_bounding_box = true;
			switch (e)
			{
			case(0):	
				configuration.test_type = TEST_TYPE::time_test;
				break;
			case(1):
				configuration.test_type = TEST_TYPE::target_test;
				configuration.use_target = true; // make sure this is enabled
				break;
			default:
				break;
			}
		}

		
	ImGui::End();
	
	ImGui::Begin("Previous Results", NULL);

	// display generation parameters and results, time, sizes, eta, methods etc, maybe store all previously generated structures as well,  have them viewable?
	if (ImGui::BeginTable("Generation Info", 14))
	{
		ImGui::TableSetupColumn("Size");
		ImGui::TableSetupColumn("Eta");
		ImGui::TableSetupColumn("Time (ms)");
		ImGui::TableSetupColumn("Grid Steps");
		ImGui::TableSetupColumn("Segment Count");
		ImGui::TableSetupColumn("Cell Selection");	
		ImGui::TableSetupColumn("Multithreading Enabled");
		ImGui::TableSetupColumn("Resetting Volume");
		ImGui::TableSetupColumn("Gradient Tolerance");
		ImGui::TableSetupColumn("Using Loop Cap");
		ImGui::TableSetupColumn("Max Loops");
		ImGui::TableSetupColumn("Using Target");
		ImGui::TableSetupColumn("Target Coords");
		ImGui::TableSetupColumn("Target Weight");
		ImGui::TableHeadersRow();


		for (auto x : tests)
		{
			ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(
					"%d, %d, %d",
					x.conditions.x_size,
					x.conditions.y_size,
					x.conditions.z_size);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d",x.conditions.eta);
				ImGui::TableSetColumnIndex(2);
				ImGui::Text(" %.0f", x.time);
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%d",x.grid_steps);
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%d", x.number_of_segments);
				ImGui::TableSetColumnIndex(5);
				ImGui::Text("%s", CandidateSelectionToString(x.conditions.candidates_from_air));
				ImGui::TableSetColumnIndex(6);
				ImGui::Text("%s", x.conditions.multithreading_enabled ? "True" : "False");
				ImGui::TableSetColumnIndex(7);
				ImGui::Text("%s", x.conditions.resetting_volume ? "True" : "False");
				ImGui::TableSetColumnIndex(8);
				ImGui::Text("%.3f", x.conditions.gradient_tolerance);
				ImGui::TableSetColumnIndex(9);
				ImGui::Text("%s",  x.conditions.loop_cap_enabled ? "True" : "False");
				ImGui::TableSetColumnIndex(10);
				ImGui::Text("%d", x.conditions.max_loops);
				//
				ImGui::TableSetColumnIndex(11);
				ImGui::Text("%s", x.conditions.using_target ? "True" : "False");
				ImGui::TableSetColumnIndex(12);
				ImGui::Text(
					"%d, %d",
					x.conditions.target_x,
					x.conditions.target_z);
				ImGui::TableSetColumnIndex(13);
				ImGui::Text("%.2f", x.conditions.target_weight);				

		}
		ImGui::EndTable();
	}

	ImGui::End();

    rlImGuiEnd();
}


const char* Controller::CandidateSelectionToString(bool candidates_from)
{
	if (candidates_from)
	{
		return "Air";
	}

	return "Lightning";
}