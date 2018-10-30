
typedef pcl::PointXYZI PointType;

cv::Point project(const pcl::PointXYZ &pt, const cv::Mat &projection_matrix)
{
    //cv::Point2f xy = projectf(pt, projection_matrix);
    cv::Mat pt_3D(4, 1, CV_32FC1);

    pt_3D.at<float>(0) = pt.x;
    pt_3D.at<float>(1) = pt.y;
    pt_3D.at<float>(2) = pt.z;
    pt_3D.at<float>(3) = 1.0f;

    cv::Mat pt_2D = projection_matrix * pt_3D;

    float w = pt_2D.at<float>(2);
    float x = pt_2D.at<float>(0) / w;
    float y = pt_2D.at<float>(1) / w;
    return cv::Point(x, y);
}

cv::Mat project(cv::Mat projection_matrix, cv::Rect frame, pcl::PointCloud<pcl::PointXYZ> point_cloud, pcl::PointCloud<pcl::PointXYZ> *visible_points)
{
    cv::Mat plane = cv::Mat::zeros(frame.size(), CV_32FC1);

    for (pcl::PointCloud<pcl::PointXYZ>::iterator pt = point_cloud.points.begin(); pt < point_cloud.points.end(); pt++)
    {

        // behind the camera
        if (pt->z < 0)
        {
            continue;
        }

        //float intensity = pt->intensity;
        cv::Point xy = project(*pt, projection_matrix);
        if (xy.inside(frame))
        {
            if (visible_points != NULL)
            {
                visible_points->push_back(*pt);
            }

            //cv::circle(plane, xy, 3, intensity, -1);
            //plane.at<float>(xy) = intensity;
            plane.at<float>(xy)=250;
        }
    }

    cv::Mat plane_gray;
    cv::normalize(plane, plane_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::dilate(plane_gray, plane_gray, cv::Mat());

    return plane_gray;
}

pcl::PointCloud<pcl::PointXYZ>* toPointsXYZ(pcl::PointCloud<PointType> point_cloud)
{
    pcl::PointCloud<pcl::PointXYZ> *new_cloud = new pcl::PointCloud<pcl::PointXYZ>();
    for (pcl::PointCloud<PointType>::iterator pt = point_cloud.points.begin(); pt < point_cloud.points.end(); pt++)
    {
        new_cloud->push_back(pcl::PointXYZ(pt->x, pt->y, pt->z));
    }
    return new_cloud;
}

