#include "mav_active_3d_planning/trajectory_evaluator.h"
#include "mav_active_3d_planning/module_factory.h"

#include <vector>

namespace mav_active_3d_planning {

    void TrajectoryEvaluator::setupFromParamMap(Module::ParamMap *param_map) {
        // Get the args to build the modules, default is a namespace extension
        std::string ns = (*param_map)["param_namespace"];
        setParam<std::string>(param_map, "cost_computer_args", &p_cost_args_, ns + "/cost_computer");
        setParam<std::string>(param_map, "value_computer_args", &p_value_args_, ns + "/value_computer");
        setParam<std::string>(param_map, "next_selector_args", &p_next_args_, ns + "/next_selector");
        setParam<std::string>(param_map, "evaluator_updater_args", &p_updater_args_, ns + "/evaluator_updater");
        std::string bounding_volume_args;
        setParam<std::string>(param_map, "bounding_volume_args", &bounding_volume_args, ns + "/bounding_volume");
        bounding_volume_ = ModuleFactory::Instance()->createModule<defaults::BoundingVolume>(bounding_volume_args,
                                                                                             verbose_modules_);
    }

    bool TrajectoryEvaluator::computeCost(TrajectorySegment *traj_in) {
        // If not implemented use a (default) module
        if (!cost_computer_) {
            cost_computer_ = ModuleFactory::Instance()->createModule<CostComputer>(p_cost_args_, verbose_modules_);
        }
        return cost_computer_->computeCost(traj_in);
    }

    bool TrajectoryEvaluator::computeValue(TrajectorySegment *traj_in) {
        // If not implemented use a (default) module
        if (!value_computer_) {
            value_computer_ = ModuleFactory::Instance()->createModule<ValueComputer>(p_value_args_, verbose_modules_);
        }
        return value_computer_->computeValue(traj_in);
    }

    int TrajectoryEvaluator::selectNextBest(const TrajectorySegment &traj_in) {
        // If not implemented use a (default) module
        if (!next_selector_) {
            next_selector_ = ModuleFactory::Instance()->createModule<NextSelector>(p_next_args_, verbose_modules_);
        }
        return next_selector_->selectNextBest(traj_in);
    }

    bool TrajectoryEvaluator::updateSegments(TrajectorySegment *root) {
        // If not implemented use a (default) module
        if (!evaluator_updater_) {
            evaluator_updater_ = ModuleFactory::Instance()->createModule<EvaluatorUpdater>(p_updater_args_,
                                                                                           verbose_modules_, this);
        }
        return evaluator_updater_->updateSegments(root);
    }

    void TrajectoryEvaluator::setVoxbloxPtr(const std::shared_ptr <voxblox::EsdfServer> &voxblox_ptr) {
        voxblox_ptr_ = voxblox_ptr;
    }

    void EvaluatorUpdater::setParent(Module *parent) {
        parent_ = dynamic_cast<TrajectoryEvaluator*>(parent);
    }

} // namepsace mav_active_3d_planning