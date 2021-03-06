#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <queue>

class Segmenter
{
public:
	Segmenter(std::string outputFolder = "dont_write_output");
	~Segmenter();

	// shape is first element, letter is second element
	std::vector<cv::Mat> segment(cv::Mat image);
	std::vector<cv::Mat> segment(string path);

private:

	struct Params 
	{
		Params()
		{
			shape_area_threshold = 0.05;
			letter_area_threshold = 0.05;
			border_threshold = 0.2;
			max_blobs_allowed = 10;
			max_clusters = 3;
			max_num_kmeans_iter = 40;
			max_kmeans_epsilon = 1.0;
			fixed_image_width = 150;   
			kmeans_terminator = cv::TermCriteria(
				cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
				max_num_kmeans_iter,
				max_kmeans_epsilon
				);
		}

		// as a ratio out of 1 (e.g letter threshold is 0.05 = 5% of shape area)
		float shape_area_threshold;
		float letter_area_threshold;
		float border_threshold;

		// other parameters
		int max_blobs_allowed;
		int fixed_image_width;
		int max_clusters;
		int max_num_kmeans_iter;
		double max_kmeans_epsilon;
		std::string outputFolder;
		cv::TermCriteria kmeans_terminator;
	};

	Params params;

	// if false positive, return empty matrices (emptyMat, emptyMat)
	// if only a promising shape is found, returns (shape, emptyMat) in pair
	// if both shape and letter are found, returns (shape, letter) in pair
	// pixel = 1 associated with shape/letter, pixel != 1 otherwise (might not be 0)
	std::pair<cv::Mat, cv::Mat> analyzeLabels(cv::Mat kmeans_labels, int num_rows);

	// argument Mat 'shape' formatting: 
	// all shape pixels = 1, other pixels associated with label = 2 (optional), everything else = 0
	// returns a Mat with all letter pixels = 1, everything else = 0
	// note that an emptyMat may be returned if nothing is found 
	// the bool indicates whether this shape may be a false positive
	std::pair<cv::Mat, bool> retrieveLetter(cv::Mat& shape, int shape_area);

	// convenience function: 
	// flood fills into all pixels == 1, reporting (area, edge_touches) in return
	// saves the flood fill result in Mat canvas by reference
	std::pair<int, int> floodFill(cv::Mat& canvas, int i, int j);

	// the fill_id must be unique for each flood filling run, fills into all pixels = 1
	void floodFill(cv::Mat& canvas, int& area, int& edge_touches, int i, int j, int fill_id);

	// take only the largest contiguous, non-zero piece
	void removeNoise(cv::Mat& label);

	cv::Mat isolateLabel(cv::Mat mylabel, int num_rows, int label_num);
};

