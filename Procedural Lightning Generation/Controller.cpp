#include "Controller.h"


void Controller::init(bool dark_mode)
{
    rlImGuiSetup(dark_mode);
}

void Controller::render(Config & configuration)
{
    rlImGuiBegin();
	ImPlot3D::CreateContext();

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

			ImGui::SliderInt("Eta", &configuration.eta, 1, 10);	

		//	ImGui::Checkbox("Include border cells in potential calculation", &configuration.include_border_cells); // this causes weird stuff, user can no longer change it

			ImGui::Checkbox("Use Target", &configuration.use_target);

			if (configuration.use_target)
			{

				ImGui::SliderInt("Target X", &configuration.target_x, 0, configuration.x_size );
				ImGui::SliderInt("Target Z", &configuration.target_z, 0, configuration.z_size);

				configuration.target_y = configuration.y_size;

				ImGui::SliderFloat("Weighting", &configuration.target_lambda, 0, 0.1);
			}


			//ImGui::Text("Starting Charges - In Progress");

		/*	std::vector<int> starting_charges;
			starting_charges.resize(configuration.x_size * configuration.z_size);

			const float size = 3;
			for (int y = 0; y < configuration.z_size; y++)
			{
				for (int x = 0; x < configuration.x_size; x++)
				{
					if (x > 0)
						ImGui::SameLine();
					ImGui::PushID(y * configuration.z_size + x);
					if (ImGui::Selectable("", starting_charges[y * configuration.z_size + x] != 0, 0, ImVec2(size, size)))
					{
						starting_charges[y * configuration.z_size + x] ^= 1;
					}
					ImGui::PopID();
				}
			}*/

			//
			//configuration.starting_charges.resize(configuration.x_size * configuration.z_size);

			//const float size = 5.0f;

			//ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

			//for (int y = 0; y < configuration.z_size; y++)
			//{
			//	for (int x = 0; x < configuration.x_size; x++)
			//	{
			//		if (x > 0)
			//			ImGui::SameLine();

			//		int index = y * configuration.x_size + x;
			//		bool selected = configuration.starting_charges[index] != 0;

			//		ImGui::PushID(index);

			//		if (selected)
			//		{
			//			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.9f, 0.2f, 1.0f)); // yellow
			//			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.95f, 0.4f, 1.0f));
			//			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.8f, 0.1f, 1.0f));
			//		}
			//		else
			//		{
			//			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.0f)); // grey
			//			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
			//			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.65f, 0.65f, 0.65f, 1.0f));
			//		}

			//		if (ImGui::Selectable("", selected, 0, ImVec2(size, size)))
			//		{
			//			configuration.starting_charges[index] ^= 1;
			//		}

			//		ImGui::PopStyleColor(3);
			//		ImGui::PopID();
			//	}
			//}

			//ImGui::PopStyleVar();

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

		ImGui::SeparatorText("Timing test controls");
		ImGui::SliderInt("Dimension increment", &configuration.dimension_increment, 1, 10);
		ImGui::SliderInt("Generation times per dimension", &configuration.number_to_average, 1, 10);

		ImGui::SeparatorText("Target test controls");
		ImGui::InputInt("Number to test", &configuration.targets_to_test, 1, 10);


		ImGui::Text("Test Type");
		static int e = 0;
		ImGui::RadioButton("Timing Test", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Target Test", &e, 1); 

		if (ImGui::Button("Perform Test"))
		{
			configuration.is_perform_test = true;
			switch (e)
			{
			case(0):	
				configuration.test_type = TEST_TYPE::time_test;
				configuration.x_size = 5;
				configuration.y_size = 5;
				configuration.z_size = 5;
				configuration.is_bounding_box = true;
				break;
			case(1):
				configuration.test_type = TEST_TYPE::target_test;
				configuration.use_target = true; // make sure this is enabled
				break;
			default:
				break;
			}
		}

		if (!configuration.xs.empty())
		{
			if (ImPlot3D::BeginPlot("X & Y") ){

				ImPlot3D::SetupAxesLimits(
					0, 30,        // X
					0, 30,        // Y
					0, 1600     // Z (time)
				);

				ImPlot3D::PlotScatter("X & Y", configuration.xs.data() , configuration.ys.data(), configuration.avg_times.data(), configuration.xs.size());
		
				ImPlot3D::EndPlot();
			}

			if (ImPlot3D::BeginPlot("X&Z")) {

				ImPlot3D::SetupAxesLimits(
					0, 30,        // X
					0, 30,        // Y
					0, 1600     // Z (time)
				);

				ImPlot3D::PlotScatter("X & Z", configuration.xs.data(), configuration.zs.data(), configuration.avg_times.data(), configuration.xs.size());
				ImPlot3D::EndPlot();
			}

			if (ImPlot3D::BeginPlot("Y&Z")) {

				ImPlot3D::SetupAxesLimits(
					0, 30,        // X
					0, 30,        // Y
					0, 1600     // Z (time)
				);

				ImPlot3D::PlotScatter("Y & Z", configuration.ys.data(), configuration.zs.data(), configuration.avg_times.data(), configuration.xs.size());
				ImPlot3D::EndPlot();
			}
		}
		
	
	ImGui::End();
	
	ImGui::Begin("Previous Results", NULL);

	if (ImGui::Button("Write data to file (will overwrite previous)"))
	{
		std::ofstream data_file("data.txt");

		for (auto x : configuration.saved_info) // todo: write data to file
		{
			data_file << "Size: " << x.x_size << "," << x.y_size << "," << x.z_size << " Eta:" << std::fixed << x.eta << " Time:" << std::fixed << x.time << "ms" << " Grid Steps:" << std::fixed << x.grid_steps << "\n";
		}

		data_file.close();
	}

	// display generation parameters and results, time, sizes, eta, methods etc, maybe store all previously generated structures as well,  have them viewable? 

	// todo:: add targeting stuff

	if (ImGui::BeginTable("Generation Info", 10))
	{
		ImGui::TableSetupColumn("Size");
		ImGui::TableSetupColumn("Eta");
		ImGui::TableSetupColumn("Time (ms)");
		ImGui::TableSetupColumn("Grid Steps");
		ImGui::TableSetupColumn("Cell Selection");	
		ImGui::TableSetupColumn("Multithreading Enabled");
		ImGui::TableSetupColumn("Resetting Volume");
		ImGui::TableSetupColumn("Gradient Tolerance");
		ImGui::TableSetupColumn("Using Loop Cap");
		ImGui::TableSetupColumn("Max Loops");
	
		ImGui::TableHeadersRow();


		for (auto x : configuration.saved_info)
		{
			ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(
					"%d, %d, %d",
					x.x_size,
					x.y_size,
					x.z_size);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d",x.eta);
				ImGui::TableSetColumnIndex(2);
				ImGui::Text(" %.0f", x.time);
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%d",x.grid_steps);
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%s", CandidateSelectionToString(x.candidates_from_air));
				ImGui::TableSetColumnIndex(5);
				ImGui::Text("%s", x.multithreading_enabled ? "True" : "False");
				ImGui::TableSetColumnIndex(6);
				ImGui::Text("%s", x.resetting_volume ? "True" : "False");
				ImGui::TableSetColumnIndex(7);
				ImGui::Text("%.3f", x.gradient_tolerance);
				ImGui::TableSetColumnIndex(8);
				ImGui::Text("%s",  x.loop_cap_enabled ? "True" : "False");
				ImGui::TableSetColumnIndex(9);
				ImGui::Text("%d", x.max_loops);
		}
		ImGui::EndTable();
	}



	ImGui::End();


	ImPlot3D::DestroyContext();
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