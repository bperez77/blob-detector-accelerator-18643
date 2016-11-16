% headlight_detection_test.m
%
% Saturday, November 12, 2016 at 10:31:15 PM EST
% Brandon Perez (bmperez)
% Devon White (dww)
% Yiyi Zhang (yiyiz)
%
% This script contains the headlight detector test.
%
% This uses the blob detector and has the best parameters for headlight
% detection. The script loads in several test headlight images, feeds them
% through the blob detector, then overlays the detections on the original
% image, and saves them to a new output file.

function [] = headlight_detection_test()
    % Get the static configuration parameters for the script
    [scale_factor, num_scales, monochrome_threshold, response_threshold, ...
            blob_filter] = get_config();

    % Load a headlight image from file, and run blob detection on it
    test_image_file = 'headlight_images/headlight1.jpg';
    headlight_image = im2double(imread(test_image_file));
    [bounding_boxes] = blob_detector(headlight_image, scale_factor, ...
            num_scales, monochrome_threshold, response_threshold, blob_filter);

    % Display the image with the detections (bounding boxes) overlaid on it
    plot_overlaid_image(headlight_image, bounding_boxes, test_image_file);
end

function [scale_factor, num_scales, monochrome_threshold, ...
        response_threshold, blob_filter] = get_config()
    % The amount by which to scale down the image at each level. This is the
    % factor to reduce each dimension by (e.g. width / scale_factor)
    scale_factor = 2;

    % The number of scale levels, including the original scale. The image will
    % be scaled down num_scales - 1 times.
    num_scales = 5;

    % The threshold to determine to convert an image to monochrome (bw) image.
    max_grayscale = 255;
    monochrome_threshold = 217 / max_grayscale;

    % The threshold to determine if a LoG filter response value corresponds to
    % the center of a headlight blob.
    max_grayscale = 255;
    response_threshold = 125 / max_grayscale;

    % The filter used to detect car headlight-like blobs. This is a 5x5
    % inverted Laplacian of Gaussian filter (LoG) with an sigma value of 1
    log_size = [5 5];
    log_sigma = 1;
    blob_filter = -fspecial('log', log_size, log_sigma);
end

function [] = plot_overlaid_image(image, bounding_boxes, filename)
    % If no bounding boxes were found, the overlaid image is just the original
    if size(bounding_boxes, 2) ~= 0
        % Convert the bounding boxes to row-vector (x, y, width, height) format
        left_xs = bounding_boxes(1, :)';
        top_ys = bounding_boxes(2, :)';
        widths = bounding_boxes(3, :)' - left_xs;
        heights = bounding_boxes(4, :)' - top_ys;
        rectangles = [left_xs, top_ys, widths, heights];

        % Overlay the bounding boxes on the image
        overlaid_image = insertShape(image, 'Rectangle', rectangles);
    else
        overlaid_image = image;
    end

    % Setup the plot
    figure();
    hold on;
    title(sprintf('Image %s with Headlight Detections Overlaid', filename));

    % Plot the image with the detections overlaid on it
    imshow(overlaid_image);
end
