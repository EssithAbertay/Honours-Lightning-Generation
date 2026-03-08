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

		if (ImGui::TreeNode("Camera"))
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

			ImGui::Checkbox("Display Bounding Box", &configuration.is_bounding_box);
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

			ImGui::SeparatorText("Optimisations");

			ImGui::Checkbox("Reset volume between steps", &configuration.reset_vol_between_steps);

			static int candidate_selection = 0;

			ImGui::Text("Select candidate cells from:");
			ImGui::RadioButton("Air cells", &candidate_selection, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Lightning cells", &candidate_selection, 1);

			// could just be a 0/1 true/false, but switch allows for potential future options
			switch (candidate_selection)
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

				ImGui::Checkbox("Use cacluated max loops", &configuration.use_calculated_loops);

				if (configuration.use_calculated_loops)
				{
					ImGui::SliderInt("Max Loops", &configuration.max_laplace_loops, 1, 100);
				}
			
				ImGui::TreePop();
			}
		
			ImGui::TreePop();
		}
		
	



		if (ImGui::Button("Regenerate Lightning"))
		{
			configuration.is_regenerate_this_frame = true;
		}

	ImGui::End();

	ImGui::Begin("Testing", NULL);

		ImGui::SliderInt("Dimension increment", &configuration.dimension_increment, 1, 10);
		ImGui::SliderInt("Generation times per dimension", &configuration.number_to_average, 1, 10);

		if (ImGui::Button("Perform Timing Test"))
		{
			configuration.is_perform_test = true;
			configuration.x_size = 5;
			configuration.y_size = 5;
			configuration.z_size = 5;
			configuration.is_bounding_box = true;
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


	if (ImGui::BeginTable("Generation Info", 6))
	{
		ImGui::TableSetupColumn("Size");
		ImGui::TableSetupColumn("Eta");
		ImGui::TableSetupColumn("Time (ms)");
		ImGui::TableSetupColumn("Grid Steps");
		ImGui::TableSetupColumn("Cell Selection");

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