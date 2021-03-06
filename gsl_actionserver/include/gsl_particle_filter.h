#include <tf/tf.h>
#include <tf/transform_listener.h>

#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <eigen3/Eigen/Dense>
#include <numeric>
#include <random>
#include <gsl_surge_spiral.h>

class Particle{
    public:
        Particle(double x, double y, double weight);
        ~Particle();
        double x, y, weight;
    private:
        
};

class ParticleFilter:public SurgeSpiralPT{
    public:
        ParticleFilter(ros::NodeHandle *nh);
        ~ParticleFilter();
    
        int numberOfParticles; //how many particles we are going to generate
        int maxEstimations; //how many estimations we want to keep track of
        int numberOfWindObs;
        FILE* log;
        double convergenceThr; //distance at which we consider the estimations have converged
        double Rconv; //convergence threshold for the particles
        double mu; //sensor sensibility
        double Sp; //area covered by the source
        bool firstObserv;

        std::vector<Particle> particles; //current set of particles
        std::vector<Eigen::Vector2d> estimatedLocations; //record of estimated positions
        std::vector<Eigen::Vector2d> allEstimations; //record of estimated positions
        std::vector<Eigen::Vector2d> historicWind; //record of measured wind
        ros::Time lastWindObservation;

        ros::Publisher particle_markers;
        ros::Publisher estimation_markers;
        ros::Publisher average_estimation_marker;

        //plume-tracking logic
        void windCallback(const olfaction_msgs::anemometerPtr& msg) override;
        void checkState() override;

        int iterationsToConverge;
        
        //aux functions
        bool cellIsFree(double x, double y);
        Eigen::Vector2d average_vector(std::vector<Eigen::Vector2d> &data);
        visualization_msgs::Marker emptyMarker();
        Eigen::Vector2d standardDeviationWind(int first);
        Eigen::Vector2d sourceLocalizationEstimation();

        //particle filter
        void estimateLocation();
        void updateWeights(bool hit);
        bool isDegenerated();
        void resample();
        double probability(bool hit, Particle &particle);
        void generateParticles(visualization_msgs::Marker &points);
        bool particlesConverge();
        int checkSourceFound() override;
        void save_results_to_file(int result) override;
        Eigen::Vector3d valueToColor(double val);
};