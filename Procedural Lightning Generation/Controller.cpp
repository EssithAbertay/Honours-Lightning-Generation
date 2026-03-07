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

		ImGui::Checkbox("Reset volume between steps", &configuration.reset_vol_between_steps);

		static int candidate_selection = 0;

		ImGui::Text("Select candidates cells from:");
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


	    ImGui::SliderInt("Eta", &configuration.eta, 1, 10);

		ImGui::SliderFloat("Gradient Tolerance", &configuration.gradient_tolerance, 0, 1, "%.3f");

		ImGui::Checkbox("Bounding Box", &configuration.is_bounding_box);

		static int method_val = 0;

		ImGui::RadioButton("Unoptimised", &method_val, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Optimised", &method_val, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Multithreaded + Optimised", &method_val, 2);
		ImGui::SameLine();
		ImGui::RadioButton("Other", &method_val, 3);


		configuration.method = static_cast<GENERATION_METHOD>(method_val);




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
			data_file << "Size: " << x.x_size << "," << x.y_size << "," << x.z_size << " Eta:" << std::fixed << x.eta << " Method: " << std::fixed << MethodToString(x.method_used) << " Time:" << std::fixed << x.time << "ms" << " Grid Steps:" << std::fixed << x.grid_steps << "\n";
		}

		data_file.close();
	}

	// display generation parameters and results, time, sizes, eta, methods etc, maybe store all previously generated structures as well,  have them viewable? 


	if (ImGui::BeginTable("Generation Info", 6))
	{
		ImGui::TableSetupColumn("Size");
		ImGui::TableSetupColumn("Eta");
		ImGui::TableSetupColumn("Method");
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
				ImGui::Text("%s", MethodToString(x.method_used));
				ImGui::TableSetColumnIndex(3);
				ImGui::Text(" %.0f", x.time);
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%d",x.grid_steps);

				ImGui::TableSetColumnIndex(5);
				ImGui::Text("%s", CandidateSelectionToString(x.candidates_from_air));
			
		}
		ImGui::EndTable();
	}



	ImGui::End();


	ImPlot3D::DestroyContext();
    rlImGuiEnd();
}

const char* Controller::MethodToString(GENERATION_METHOD m)
{
	switch (m)
	{
	case GENERATION_METHOD::unoptimised:    return "Unoptimised";
	case GENERATION_METHOD::optimised:		return "Optimised";
	case GENERATION_METHOD::multithread:    return "Multithreaded";
	case GENERATION_METHOD::other:			return "Other";
	default:								return "Unknown";
	}
}

const char* Controller::CandidateSelectionToString(bool candidates_from)
{
	if (candidates_from)
	{
		return "Air";
	}

	return "Lightning";
}