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
	    ImGui::SliderInt("X Size", &configuration.x_size, 5, 30);
	    ImGui::SliderInt("Y Size", &configuration.y_size, 5, 30);
	    ImGui::SliderInt("Z Size", &configuration.z_size, 5, 30);

	    ImGui::SliderInt("Eta", &configuration.eta, 1, 10);

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
	
	ImPlot3D::DestroyContext();
    rlImGuiEnd();
}
