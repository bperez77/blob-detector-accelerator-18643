% blob_detector.m
%
% Saturday, November 12, 2016 at 03:52:18 PM EST
% Brandon Perez (bmperez)
% Devon White (dww)
% Yiyi Zhang (yiyiz)
%
% This script contains the implementation of the multi-scale blob detector.
%
% This script performs multi-scale blob detection on the given input RGB image.
% The image is converted to grayscale and then rescaled multiple times to create
% scale levels as per the user parameters. At each scale, the user-specified
% filter applied to the image (typically a Laplacian of Gaussian) to detect a
% specific type of blobs.
%
%
% This filter will give the centerpoint of the blob. Then, the bounding box of
% the blob is determined based on the size of the filter. This is then
% transformed back to the original image space, and a list of bounding boxes is
% returned (each column is the 4 coordinate values).

function [bounding_boxes] = blob_detector(image, scale_factor, num_scales, ...
        response_threshold, blob_filter)
    % Convert the image to a grayscale
    gray_image = rgb2gray(image);

    % Perform blob detection at the original image scale
    [bounding_boxes] = detect_blobs(gray_image, response_threshold, ...
            blob_filter);

    % Iterate over each scale level, and perform blob detection at each scale
    bounding_boxes = [];
    for scale_level = 1:num_scales-1
        % Scale the image down using bicubic interpolation
        downscale = (1 / scale_factor) .^ scale_level;
        resized_image = imresize(gray_image, downscale, 'method', 'bicubic');

        % Perform blob detection on the downscaled image
        [detections] = detect_blobs(resized_image, response_threshold, ...
                blob_filter);

        % Transform the box coordinates back to the original scale space
        scaled_boxes = (1 / downscale) * detections;
        bounding_boxes = [bounding_boxes, scaled_boxes];
    end
end

function [bounding_boxes] = detect_blobs(image, response_threshold, blob_filter)
    % Apply the blob filter, and threshold the response to determine the blobs
    blob_responses = imfilter(image, blob_filter);
    blob_responses(blob_responses < response_threshold) = 0;

    % Get the centerpoints of the blobs in the image, and compute the radius
    [center_xs, center_ys] = find(blob_responses > 0);
    center_points = [center_xs'; center_ys'];
    box_radius = ceil(size(blob_filter) / 2)';

    % Compute the top-left and bottom-right points of the boxes
    box_radius_matrix = box_radius * ones(1, size(center_points, 2));
    top_left_points = center_points - box_radius_matrix;
    bottom_right_points = center_points + box_radius_matrix;

    % Format the bounding boxes in column-vector format (x1, y1, x2, y2)
    bounding_boxes = [top_left_points; bottom_right_points];
end
